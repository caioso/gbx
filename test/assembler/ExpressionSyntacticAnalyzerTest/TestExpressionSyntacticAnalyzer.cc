#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "ExpressionIntermediateRepresentation.h"
#include "ExpressionSyntacticAnalyzer.h"
#include "GBXAsmExceptions.h"
#include "LexicalAnalyzer.h"
#include "Lexemes.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::frontend::parsers;
using namespace gbxasm::utilities;
using namespace std;

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryPlusOperationLexicalAnalysis)
{
    const string expression = "A + B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorPLUS, TokenType::Identifier}; // A + B

    auto lexemes = { "A", Lexemes::OperatorPLUS.c_str(), "B" }; // A + B

    auto columns = { 1llu, 3llu, 5llu }; // A + B
    
    auto lines = { 1llu, 1llu, 1llu }; // A + B


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpression)
{
    const string expression = "A + B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationHLCheck)
{
    const string expression = "A + B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck)
{
    const string expression = "A + B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("B", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("+", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseMalformedSimpleBinaryExpression)
{
    const string expression = "A +";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseMalformedSimpleBinaryExpression2)
{
    const string expression = "+ A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseMalformedSimpleBinaryExpression3)
{
    const string expression = "+";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseMalformedSimpleBinaryExpression4)
{
    const string expression = "A B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseMalformedSimpleBinaryExpression5)
{
    const string expression = "";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryMinusOperationLexicalAnalysis)
{
    const string expression = "A - B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorMINUS, TokenType::Identifier}; // A - B

    auto lexemes = { "A", Lexemes::OperatorMINUS.c_str(), "B" }; // A - B

    auto columns = { 1llu, 3llu, 5llu }; // A - B
    
    auto lines = { 1llu, 1llu, 1llu }; // A - B


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck2)
{
    const string expression = "A - B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("B", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("-", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryMultiplicationOperationLexicalAnalysis)
{
    const string expression = "A * B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorMULTIPLICATION, TokenType::Identifier}; // A * B

    auto lexemes = { "A", Lexemes::OperatorMULTIPLICATION.c_str(), "B" }; // A * B

    auto columns = { 1llu, 3llu, 5llu }; // A * B
    
    auto lines = { 1llu, 1llu, 1llu }; // A * B


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck3)
{
    const string expression = "A * B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("B", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("*", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryDivisionOperationLexicalAnalysis)
{
    const string expression = "A / B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorDIVISION, TokenType::Identifier}; // A / B

    auto lexemes = { "A", Lexemes::OperatorDIVISION.c_str(), "B" }; // A / B

    auto columns = { 1llu, 3llu, 5llu }; // A / B
    
    auto lines = { 1llu, 1llu, 1llu }; // A / B


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck4)
{
    const string expression = "A / B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("B", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("/", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryRightShiftOperationLexicalAnalysis)
{
    const string expression = "A >> B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorRIGHTSHIFT, TokenType::Identifier}; // A >> B

    auto lexemes = { "A", Lexemes::OperatorRIGHTSHIFT.c_str(), "B" }; // A >> B

    auto columns = { 1llu, 3llu, 6llu }; // A >> B
    
    auto lines = { 1llu, 1llu, 1llu }; // A >> B


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck5)
{
    const string expression = "A >> B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("B", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ(">>", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryRightShiftOperationLexicalAnalysis2)
{
    const string expression = "A >> 0x88";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorRIGHTSHIFT, TokenType::LiteralNumericHEXADECIMAL}; // A >> 0x88

    auto lexemes = { "A", Lexemes::OperatorRIGHTSHIFT.c_str(), "0x88" }; // A >> 0x88

    auto columns = { 1llu, 3llu, 6llu }; // A >> 0x88
    
    auto lines = { 1llu, 1llu, 1llu }; // A >> 0x88


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck6)
{
    const string expression = "A >> 0x88";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("0x88", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ(">>", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryRightShiftOperationLexicalAnalysis3)
{
    const string expression = "0o41 >> A";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::LiteralNumericOCTAL, TokenType::OperatorRIGHTSHIFT, TokenType::Identifier}; // 0o41 >> A

    auto lexemes = { "0o41", Lexemes::OperatorRIGHTSHIFT.c_str(), "A" }; // 0o41 >> A

    auto columns = { 1llu, 6llu, 9llu }; // 0o41 >> A
    
    auto lines = { 1llu, 1llu, 1llu }; // 0o41 >> A


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck7)
{
    const string expression = "0o41 >> A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ(">>", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("0o41", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryLeftShiftOperationLexicalAnalysis)
{
    const string expression = "A << B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorLEFTSHIFT, TokenType::Identifier}; // A << B

    auto lexemes = { "A", Lexemes::OperatorLEFTSHIFT.c_str(), "B" }; // A << B

    auto columns = { 1llu, 3llu, 6llu }; // A << B
    
    auto lines = { 1llu, 1llu, 1llu }; // A << B


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck8)
{
    const string expression = "A << 20";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("20", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("<<", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryLessThanOperationLexicalAnalysis)
{
    const string expression = "A < B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorLESSTHAN, TokenType::Identifier}; // A < B

    auto lexemes = { "A", Lexemes::OperatorLESSTHAN.c_str(), "B" }; // A < B

    auto columns = { 1llu, 3llu, 5llu }; // A < B
    
    auto lines = { 1llu, 1llu, 1llu }; // A < B


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck9)
{
    const string expression = "A < B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("B", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("<", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryLessThanEqualOperationLexicalAnalysis)
{
    const string expression = "A <= 0b11";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorLESSTHANOREQUALTO, TokenType::LiteralNumericBINARY}; // A <= 0b11

    auto lexemes = { "A", Lexemes::OperatorLESSTHANOREQUALTO.c_str(), "0b11" }; // A <= 0b11

    auto columns = { 1llu, 3llu, 6llu }; // A <= 0b11
    
    auto lines = { 1llu, 1llu, 1llu }; // A <= 0b11


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck10)
{
    const string expression = "A <= 0b11";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(3llu, expressionRepresentation->ExpressionStack().size());
    EXPECT_EQ(1llu, expressionRepresentation->Line());
    EXPECT_EQ(1llu, expressionRepresentation->Column());

    auto stack = expressionRepresentation->ExpressionStack();
    auto representation = stack.top();
    EXPECT_STREQ("0b11", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("<=", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}