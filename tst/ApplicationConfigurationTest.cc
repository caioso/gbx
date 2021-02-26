#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <string>

#include "TestUtils.h"

#include "../src/GBXCommonsExceptions.h"
#include "../src/ArgumentsParser.h"

using namespace std;
using namespace gbxcommons;

using ::testing::Return;
using ::testing::_;

TEST(CommandLineArgumentsParser, Construction)
{
    auto parser = make_shared<ArgumentsParser>();
}

TEST(CommandLineArgumentsParser, OptionalFlagArgumentParsing)
{
    // Constants
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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

TEST(CommandLineArgumentsParser, RequiredFlagArgumentParsing)
{
    // Constants
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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

TEST(CommandLineArgumentsParser, DoNotProvideRequiredFlagArgumentParsing)
{
    // Setup object
    auto parser = make_shared<ArgumentsParser>();
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Required);

    // Parse
    char* arguments[] = {};
    int count = 0;

    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -v/--verbose is required");
}

TEST(CommandLineArgumentsParser, OptionalPairArgumentParsing)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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

TEST(CommandLineArgumentsParser, ParseUnknownFlagArgument)
{
    // Constants
    char* unknownFlag = strdup("-f");
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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

TEST(CommandLineArgumentsParser, RetrieveNonParsedArgument)
{
    // Constants
    char* verboseFlag = strdup("-v");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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


TEST(CommandLineArgumentsParser, RetrieveNonParsedArgument2)
{
    // Constants
    char* unknownFlag = strdup("-f");
    char* unknownFlagValue = strdup("value");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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

TEST(CommandLineArgumentsParser, RetrieveNonParsedArgument3)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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

TEST(CommandLineArgumentsParser, RetrieveNonParsedArgument4)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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

TEST(CommandLineArgumentsParser, DoNotProvideRequiredFlagArgument)
{
    // Constants
    char* flag = strdup("-f");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
    parser->RegisterOption("-v", "--verbose", "Enable verbose", OptionType::Flag, OptionRequirement::Required);

    // Parse
    char* arguments[] = {flag};
    int count = 1;

    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -v/--verbose is required");
}

TEST(CommandLineArgumentsParser, RequiredPairArgumentParsing)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
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


TEST(CommandLineArgumentsParser, DoNotProvideRequiredPairArgumentParsing)
{
    // Constants
    char* otherFlag = strdup("-z");
    char* otherValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Required);

    // Parse
    char* arguments[] = {otherFlag, otherValue};
    int count = 2;

    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -i/--ip is required");
}

TEST(CommandLineArgumentsParser, DoNotProvideValueToPairOption)
{
    // Constants
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.1.1");

    // Setup object
    auto parser = make_shared<ArgumentsParser>();
    parser->RegisterOption("-i", "--ip", "Server IP address", OptionType::Pair, OptionRequirement::Optional);

    // Parse
    char* arguments[] = {ipFlag, ipValue};
    int count = 1;
    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Option -i/--ip requires a value");
}

/*
TEST(CommandLineArgumentsParser, EnableVerboseMode)
{
    char* verboseFlag = strdup("-v");
    char* arguments[] = {verboseFlag};
    int count = 1;
    auto parser = make_shared<ArgumentsParser>();
    
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    auto applicationConfiguration = parser->Configuration();

    EXPECT_TRUE(applicationConfiguration.Verbose);
}

TEST(CommandLineArgumentsParser, EnableVerboseModeExtended)
{
    char* verboseFlag = strdup("--verbose");
    char* arguments[] = {verboseFlag};
    int count = 1;
    auto parser = make_shared<ArgumentsParser>();
    
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    auto applicationConfiguration = parser->Configuration();

    EXPECT_TRUE(applicationConfiguration.Verbose);
}

TEST(CommandLineArgumentsParser, SetUpDebugModeWithIPAndPort)
{
    char* debugFlag = strdup("-d");
    char* ipFlag = strdup("-i");
    char* ipValue = strdup("192.168.0.1");
    char* portFlag = strdup("-p");
    char* portValue = strdup("5555");
    char* arguments[] = {debugFlag, ipFlag, ipValue, portFlag, portValue};
    int count = 5;
    auto parser = make_shared<ArgumentsParser>();
    
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    auto applicationConfiguration = parser->Configuration();

    EXPECT_TRUE(applicationConfiguration.IsDebug);
    EXPECT_STREQ("192.168.0.1", applicationConfiguration.IPAddress.c_str());
    EXPECT_STREQ("5555", applicationConfiguration.Port.c_str());
}

TEST(CommandLineArgumentsParser, SetUpDebugModeWithIPAndPortExtended)
{
    char* debugFlag = strdup("--debug");
    char* ipFlag = strdup("--ip");
    char* ipValue = strdup("127.0.0.1");
    char* portFlag = strdup("--port");
    char* portValue = strdup("9999");
    char* arguments[] = {debugFlag, ipFlag, ipValue, portFlag, portValue};
    int count = 5;
    auto parser = make_shared<ArgumentsParser>();
    
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    auto applicationConfiguration = parser->Configuration();

    EXPECT_TRUE(applicationConfiguration.IsDebug);
    EXPECT_STREQ("127.0.0.1", applicationConfiguration.IPAddress.c_str());
    EXPECT_STREQ("9999", applicationConfiguration.Port.c_str());
}

TEST(CommandLineArgumentsParser, MalformedCommandLineOptionTest1)
{
    char* ipFlag = strdup("--ip");
    char* ipValue = strdup("127.0.0.1");
    char* portFlag = strdup("--port");
    char* portValue = strdup("9999");
    char* arguments[] = {ipFlag, ipValue, portFlag, portValue};
    int count = 4;
    auto parser = make_shared<ArgumentsParser>();
    
    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "-d/--debug option expected");
}

TEST(CommandLineArgumentsParser, MalformedCommandLineOptionTest2)
{
    char* debugFlag = strdup("--debug");
    char* ipFlag = strdup("--ip");
    char* portFlag = strdup("--port");
    char* portValue = strdup("9999");
    char* arguments[] = {debugFlag, ipFlag, portFlag, portValue};
    int count = 4;
    auto parser = make_shared<ArgumentsParser>();
    
    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Invalid IP Address '--port'");
}

TEST(CommandLineArgumentsParser, MalformedCommandLineOptionTest3)
{
    char* debugFlag = strdup("--debug");
    char* portFlag = strdup("--port");
    char* arguments[] = {debugFlag, portFlag, debugFlag};
    int count = 3;
    auto parser = make_shared<ArgumentsParser>();
    
    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "Invalid Port '--debug'");
}

TEST(CommandLineArgumentsParser, MalformedCommandLineOptionTest4)
{
    char* debugFlag = strdup("--debug");
    char* ipFlag = strdup("--ip");
    char* ipValue = strdup("127.0.0.1");
    char* arguments[] = {debugFlag, ipFlag, ipValue};
    int count = 3;
    auto parser = make_shared<ArgumentsParser>();
    
    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "-p/--port option required");
}

TEST(CommandLineArgumentsParser, MalformedCommandLineOptionTest5)
{
    char* debugFlag = strdup("--debug");
    char* portFlag = strdup("--port");
    char* portValue = strdup("9999");
    char* arguments[] = {debugFlag, portFlag, portValue};
    int count = 3;
    auto parser = make_shared<ArgumentsParser>();
    
    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      "-i/--ip option required");
}

TEST(CommandLineArgumentsParser, LaunchInRuntimeMode)
{
    char* arguments[] = {};
    int count = 0;
    auto parser = make_shared<ArgumentsParser>();
    
    parser->Parse(reinterpret_cast<char**>(arguments), count);
    auto applicationConfiguration = parser->Configuration();

    EXPECT_FALSE(applicationConfiguration.IsDebug);
    EXPECT_STREQ("", applicationConfiguration.IPAddress.c_str());
    EXPECT_STREQ("", applicationConfiguration.Port.c_str());
}

TEST(CommandLineArgumentsParser, helpOption)
{
    string message = "Usage: gbx [-d/--debug -i/--ip <IPAddress> -p/--port <Port>]\n"
                   "options:\n"
                   " -d/--debug\t\tEnable debug mode\n"
                   " -i/--ip <IPAddress>\tDebug Server IP Address\n"
                   " -p/--port <Port>\tDebug Server Port Number\n"
                   " -v/--verbose\t\tVerbose mode\n"
                   " -h/--help\t\tShow this message\n";

    char* helpFlag = strdup("-h");
    char* arguments[] = {helpFlag};
    int count = 1;
    auto parser = make_shared<ArgumentsParser>();
    

    ASSERT_EXCEPTION( { parser->Parse(reinterpret_cast<char**>(arguments), count); }, 
                      ArgumentsParserException, 
                      message.c_str());
}*/