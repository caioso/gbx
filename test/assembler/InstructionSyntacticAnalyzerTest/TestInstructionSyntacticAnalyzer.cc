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

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOP1AndImmediateOP2)
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

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOP1AndImmediateOP22)
{
    const string expression = "LD HL, 0o7600";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOP1AndImmediateOP23)
{
    const string expression = "LD E, 0b1010101001";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOP1AndImmediateOP24)
{
    const string expression = "LD C, 45";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseImmediateOP1AndRegisterOP2)
{
    const string expression = "LD 1, A";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseImmediateOP1AndRegisterOP21)
{
    const string expression = "LD 0x01, B";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseImmediateOP1AndRegisterOP22)
{
    const string expression = "LD 0b000000, C";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseImmediateOP1AndRegisterOP23)
{
    const string expression = "LD 0o1, D";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOP1AndRegisterOP2)
{
    const string expression = "LD B, A";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOP1AndRegisterOP22)
{
    const string expression = "LD MY_REGISTER, HL";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOP1AndRegisterOP23)
{
    const string expression = "LD A, MY_ALIAS";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseRegisterOneRegisterOP)
{
    const string expression = "INC A";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseJPWithRegisterOneLiteralOP)
{
    const string expression = "JP 0xFFFF";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseJPWithIdentifierAndLiteralOPs)
{
    const string expression = "JP NZ, 0xFFAA";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseCALLWithRegisterOneLiteralOP)
{
    const string expression = "CALL MY_LABEL";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_InstructionSyntacticAnalysis, ParseCALLWithIdentifierAndLiteralOPs)
{
    const string expression = "CALL Z, THAT_LABEL";
    LexicalAnalyzer lexer;
    InstructionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}
