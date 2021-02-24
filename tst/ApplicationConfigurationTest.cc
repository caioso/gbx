#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <string>

#include "TestUtils.h"

#include "../src/GBXExceptions.h"
#include "../src/ArgumentsParser.h"

using namespace std;
using namespace gbx;

using ::testing::Return;
using ::testing::_;

TEST(CommandLineArgumentsParser, Construction)
{
    auto parser = make_shared<ArgumentsParser>();
}


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
                      GBXException, 
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
                      GBXException, 
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
                      GBXException, 
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
                      GBXException, 
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
                      GBXException, 
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
                      GBXException, 
                      message.c_str());
}