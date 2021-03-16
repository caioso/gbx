#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <string>

#include "TestUtils.h"

#include "GBXCommonsExceptions.h"
#include "ArgumentsParser.h"

using namespace std;
using namespace gbxcommons;

using ::testing::Return;
using ::testing::_;

TEST(CommonsTests_CommandLineArgumentsParser, Construction)
{
    auto parser = make_shared<ArgumentsParser>("");
}

TEST(CommonsTests_CommandLineArgumentsParser, OptionalFlagArgumentParsing)
{
    // Constants
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {verboseFlag};
    int count = 1;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    auto verboseFound = parser->HasBeenFound("-v");
    EXPECT_TRUE(verboseFound);
    auto debugFound = parser->HasBeenFound("-d");
    EXPECT_FALSE(debugFound);
    auto parsedOption = parser->RetrieveOption("-v");
    EXPECT_STREQ("-v", parsedOption.ShortVersion.c_str());
    EXPECT_EQ(nullopt, parsedOption.Value);
}

TEST(CommonsTests_CommandLineArgumentsParser, RequiredFlagArgumentParsing)
{
    // Constants
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Required);

    // Parse
    char* arguments[] = {verboseFlag};
    int count = 1;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    auto verboseFound = parser->HasBeenFound("-v");
    EXPECT_TRUE(verboseFound);
    auto debugFound = parser->HasBeenFound("-d");
    EXPECT_FALSE(debugFound);
    auto parsedOption = parser->RetrieveOption("-v");
    EXPECT_STREQ("-v", parsedOption.ShortVersion.c_str());
    EXPECT_EQ(nullopt, parsedOption.Value);
}

TEST(CommonsTests_CommandLineArgumentsParser, DoNotProvideRequiredFlagArgumentParsing)
{
    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Required);

    // Parse
    char* arguments[] = {};
    int count = 0;

    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -v/--verbose is required");
}

TEST(CommonsTests_CommandLineArgumentsParser, OptionalPairArgumentParsing)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {ipFlag, ipValue};
    int count = 2;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    auto ipFound = parser->HasBeenFound("-i");
    EXPECT_TRUE(ipFound);
    auto parsedOption = parser->RetrieveOption("-i");
    EXPECT_STREQ("-i", parsedOption.ShortVersion.c_str());
    EXPECT_STREQ("192.168.1.1", parsedOption.Value.value_or(string("")).c_str());
}

TEST(CommonsTests_CommandLineArgumentsParser, ParseUnknownFlagArgument)
{
    // Constants
    char* unknownFlag = strdup("-f");
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {unknownFlag, verboseFlag};
    int count = 2;
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    
    // Check result
    auto verboseFound = parser->HasBeenFound("-v");
    EXPECT_TRUE(verboseFound);
    auto debugFound = parser->HasBeenFound("-f");
    EXPECT_FALSE(debugFound);
}

TEST(CommonsTests_CommandLineArgumentsParser, RetrieveNonParsedArgument)
{
    // Constants
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {verboseFlag};
    int count = 1;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    ASSERT_EXCEPTION( { parser->RetrieveOption("-z"); }, 
                      ArgumentsParserException, 
                      "Option -z has not been parsed");
}


TEST(CommonsTests_CommandLineArgumentsParser, RetrieveNonParsedArgument2)
{
    // Constants
    char* unknownFlag = strdup("-f");
    char* unknownFlagValue = strdup("value");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {unknownFlag, unknownFlagValue};
    int count = 2;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    ASSERT_EXCEPTION( { parser->RetrieveOption("-z"); }, 
                      ArgumentsParserException, 
                      "Option -z has not been parsed");
}

TEST(CommonsTests_CommandLineArgumentsParser, RetrieveHelp1)
{
    // Constants
    char* helpFlag = strdup("-h");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {helpFlag};
    int count = 1;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    auto helpFound = parser->HasBeenFound("-h");
    EXPECT_TRUE(helpFound);
}

TEST(CommonsTests_CommandLineArgumentsParser, RetrieveHelp2)
{
    // Constants
    char* helpFlag = strdup("--help");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {helpFlag};
    int count = 1;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    auto helpFound = parser->HasBeenFound("-h");
    EXPECT_TRUE(helpFound);
}


TEST(CommonsTests_CommandLineArgumentsParser, RetrieveNonParsedArgument3)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {ipFlag, ipValue};
    int count = 2;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    ASSERT_EXCEPTION( { parser->RetrieveOption("-v"); }, 
                      ArgumentsParserException, 
                      "Option -v has not been parsed");
}

TEST(CommonsTests_CommandLineArgumentsParser, RetrieveNonParsedArgument4)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {ipFlag, ipValue};
    int count = 0;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    ASSERT_EXCEPTION( { parser->RetrieveOption("-v"); }, 
                      ArgumentsParserException, 
                      "Option -v has not been parsed");
}

TEST(CommonsTests_CommandLineArgumentsParser, DoNotProvideRequiredFlagArgument)
{
    // Constants
    char* flag = strdup("-f");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Required);

    // Parse
    char* arguments[] = {flag};
    int count = 1;

    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -v/--verbose is required");
}

TEST(CommonsTests_CommandLineArgumentsParser, RequiredPairArgumentParsing)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Required);

    // Parse
    char* arguments[] = {ipFlag, ipValue};
    int count = 2;
    parser->Parse(reinterpret_cast<char**>(arguments), count);

    // Check result
    auto ipFound = parser->HasBeenFound("-i");
    EXPECT_TRUE(ipFound);
    auto parsedOption = parser->RetrieveOption("-i");
    EXPECT_STREQ("-i", parsedOption.ShortVersion.c_str());
    EXPECT_STREQ("192.168.1.1", parsedOption.Value.value_or(string("")).c_str());
}


TEST(CommonsTests_CommandLineArgumentsParser, DoNotProvideRequiredPairArgumentParsing)
{
    // Constants
    char* otherFlag = strdup("-z");
    char* otherValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Required);

    // Parse
    char* arguments[] = {otherFlag, otherValue};
    int count = 2;

    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -i/--ip is required");
}

TEST(CommonsTests_CommandLineArgumentsParser, DoNotProvideValueToPairOption)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>("");
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {ipFlag, ipValue};
    int count = 1;
    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -i/--ip requires a value");
}

TEST(CommonsTests_CommandLineArgumentsParser, DisplayHelpMessage)
{
    const string helpMessage = "USAGE: MyTool -i <input> -o <output> [-v/--verbose | -t/--timeout <timeout>]\n"
                               "----------------------------------------------------------------------------\n"
                               "OPTIONS:\n"
                               "-i/--input   <value> Input argument (required)\n"
                               "-o/--output  <value> Output argument (required)\n"
                               "-t/--timeout <value> Timeout value (optional)\n"
                               "-v/--verbose         Verbose mode (optional)\n\n"
                               "-h/--help            Display available options";

    auto parser = make_shared<ArgumentsParser>("MyTool -i <input> -o <output> [-v/--verbose | -t/--timeout <timeout>]");
    parser->RegisterOption("-i", "--input", "Input argument", OptionType::Pair, OptionRequirement::Required);
    parser->RegisterOption("-o", "--output", "Output argument", OptionType::Pair, OptionRequirement::Required);
    parser->RegisterOption("-t", "--timeout", "Timeout value", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-v", "--verbose", "Verbose mode", OptionType::Flag, OptionRequirement::Optional);

    // Setup object
    auto help = parser->Help();
    ASSERT_STREQ(helpMessage.c_str(), help.c_str());
}


TEST(CommonsTests_CommandLineArgumentsParser, EnableVerboseMode)
{
    char* verboseFlag = strdup("-v");
    char* arguments[] = {verboseFlag};
    int count = 1;

    auto parser = make_shared<ArgumentsParser>("Test Description");
    parser->RegisterOption("-d", "--debug", "Enable Debug Mode", OptionType::Flag, OptionRequirement::Optional);
    parser->RegisterOption("-i", "--ip", "IP Address", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-p", "--port", "Port Number", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-v", "--verbose", "Verbose mode", OptionType::Flag, OptionRequirement::Optional);
    
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    auto veboseFound = parser->HasBeenFound("--verbose");

    EXPECT_TRUE(veboseFound);
}

TEST(CommonsTests_CommandLineArgumentsParser, EnableVerboseModeExtended)
{
    char* verboseFlag = strdup("--verbose");
    char* arguments[] = {verboseFlag};
    int count = 1;
    
    auto parser = make_shared<ArgumentsParser>("Test Description");
    parser->RegisterOption("-d", "--debug", "Enable Debug Mode", OptionType::Flag, OptionRequirement::Optional);
    parser->RegisterOption("-i", "--ip", "IP Address", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-p", "--port", "Port Number", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-v", "--verbose", "Verbose mode", OptionType::Flag, OptionRequirement::Optional);
    
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    auto veboseFound = parser->HasBeenFound("-v");

    EXPECT_TRUE(veboseFound);
}

TEST(CommonsTests_CommandLineArgumentsParser, SetUpDebugModeWithIPAndPort)
{
    char* debugFlag = strdup("-d");
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.0.1");
    char* portFlag = strdup("-p");
    char* portValue = strdup("5555");
    char* arguments[] = {debugFlag, ipFlag, ipValue, portFlag, portValue};
    int count = 5;
    
    auto parser = make_shared<ArgumentsParser>("Test Description");
    parser->RegisterOption("-d", "--debug", "Enable Debug Mode", OptionType::Flag, OptionRequirement::Optional);
    parser->RegisterOption("-i", "--ip", "IP Address", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-p", "--port", "Port Number", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-v", "--verbose", "Verbose mode", OptionType::Flag, OptionRequirement::Optional);

    parser->Parse(reinterpret_cast<char**>(arguments), count);

    auto debugFound = parser->HasBeenFound("-d");
    EXPECT_TRUE(debugFound);

    auto ipFound = parser->HasBeenFound("-i");
    EXPECT_TRUE(ipFound);
    auto ipOption = parser->RetrieveOption("-i");
    EXPECT_STREQ("192.168.0.1", ipOption.Value.value().c_str());
    
    auto portFound = parser->HasBeenFound("-p");
    EXPECT_TRUE(portFound);
    auto portOption = parser->RetrieveOption("-p");
    EXPECT_STREQ("5555", portOption.Value.value().c_str());
}

TEST(CommonsTests_CommandLineArgumentsParser, SetUpDebugModeWithIPAndPortExtended)
{
    char* debugFlag = strdup("--debug");
    char* ipFlag = strdup("--ip");
    char* ipValue = strdup("127.0.0.1");
    char* portFlag = strdup("--port");
    char* portValue = strdup("9999");
    char* arguments[] = {debugFlag, ipFlag, ipValue, portFlag, portValue};
    int count = 5;
    
    auto parser = make_shared<ArgumentsParser>("Test Description");
    parser->RegisterOption("-d", "--debug", "Enable Debug Mode", OptionType::Flag, OptionRequirement::Optional);
    parser->RegisterOption("-i", "--ip", "IP Address", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-p", "--port", "Port Number", OptionType::Pair, OptionRequirement::Optional);
    parser->RegisterOption("-v", "--verbose", "Verbose mode", OptionType::Flag, OptionRequirement::Optional);

    parser->Parse(reinterpret_cast<char**>(arguments), count);

    auto debugFound = parser->HasBeenFound("-d");
    EXPECT_TRUE(debugFound);

    auto ipFound = parser->HasBeenFound("-i");
    EXPECT_TRUE(ipFound);
    auto ipOption = parser->RetrieveOption("-i");
    EXPECT_STREQ("127.0.0.1", ipOption.Value.value().c_str());
    
    auto portFound = parser->HasBeenFound("-p");
    EXPECT_TRUE(portFound);
    auto portOption = parser->RetrieveOption("-p");
    EXPECT_STREQ("9999", portOption.Value.value().c_str());
}