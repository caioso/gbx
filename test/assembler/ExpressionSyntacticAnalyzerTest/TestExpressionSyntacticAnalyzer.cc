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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionLexicalAnalysis)
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