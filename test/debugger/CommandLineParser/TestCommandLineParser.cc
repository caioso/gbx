#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "ClientMessageID.h"
#include "CommandLineParser.h"
#include "DebugMessage.h"
#include "TestUtils.h"

using namespace gbxdb;
using namespace gbxdb::input;
using namespace std;

using ::testing::Return;
using ::testing::_;

TEST(DebuggerTests_ComamandLineParser, Construction) 
{
    CommandLineParser parser;
}

TEST(DebuggerTests_ComamandLineParser, Initialize) 
{
    CommandLineParser parser;
    parser.Initialize();
}

TEST(DebuggerTests_ComamandLineParser, ParseUnknownCommandWithoutArgument) 
{
    const string noCommand = "no-command";
    CommandLineParser parser;
    parser.Initialize();

    ASSERT_EXCEPTION( { parser.Parse(noCommand); }, 
                      CommandLineInputException, 
                      "Unknown command 'no-command'");
}

TEST(DebuggerTests_ComamandLineParser, ParseUnknownCommandWithArgument) 
{
    const string noCommand = "no-command argument";
    CommandLineParser parser;
    parser.Initialize();

    ASSERT_EXCEPTION( { parser.Parse(noCommand); }, 
                      CommandLineInputException, 
                      "Unknown command 'no-command'");
}

TEST(DebuggerTests_ComamandLineParser, ParseDummyCommand) 
{
    const string registerBankSummaryCommand = "register-bank";
    CommandLineParser parser;
    parser.Initialize();

    auto message = parser.Parse(registerBankSummaryCommand);
    
    uint16_t messageID = (*message.Buffer())[0] | ((*message.Buffer())[1] << 0x08);
    EXPECT_EQ(ClientMessageID::MessageRegisterBankSummary, messageID);
}
