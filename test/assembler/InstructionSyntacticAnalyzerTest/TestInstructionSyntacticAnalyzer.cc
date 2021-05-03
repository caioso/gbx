#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "GBXAsmExceptions.h"
#include "InstructionSyntacticAnalyzer.h"
#include "LexicalAnalyzer.h"
#include "Lexemes.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::frontend::parsers;
using namespace gbxasm::utilities;
using namespace std;

TEST(AssemblerTests_InstructionSyntacticAnalysis, SanityCheckInstructionParsing)
{
    const string expression = "LD A, 0xFF";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::InstructionMnemonicLD, TokenType::Identifier, 
                        TokenType::SeparatorCOMMA, TokenType::LiteralNumericHEXADECIMAL}; // LD A, 0xFF

    auto lexemes = { Lexemes::InstructionMnemonicLD.c_str(), "A", Lexemes::SeparatorCOMMA.c_str(), "0xFF" }; // LD A, 0xFF

    auto columns = { 1llu, 4llu, 5llu, 7llu}; // LD A, 0xFF
    
    auto lines = { 1llu, 1llu, 1llu, 1llu}; // LD A, 0xFF

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseInstructionRegisterImmediate)
{
    const string expression = "LD A, 0xEE";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}