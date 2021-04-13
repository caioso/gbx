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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSingleIdentifier)
{
    const string expression = "A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseNothing)
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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAddition)
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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAddition2)
{
    const string expression = "A + B + C";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAddition3)
{
    const string expression = "A + B + C + D";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAddition4)
{
    const string expression = "A + + B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAddition2)
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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAddition3)
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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAddition4)
{
    const string expression = "A + B + C + + +";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAdditionWithParenthesis)
{
    const string expression = "(A + B)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAdditionWithParenthesis2)
{
    const string expression = "(A + B + C)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAdditionWithParenthesis3)
{
    const string expression = "(A + (B + C))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAdditionWithParenthesis4)
{
    const string expression = "(A + ((B) + C))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAdditionWithParenthesis5)
{
    const string expression = "(((((A))))) + ((((B)))) + ((C + (((D))) + E))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAdditionWithParenthesis)
{
    const string expression = "(A + (() + C))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAdditionWithParenthesis2)
{
    const string expression = "(A + B ( + C )";
    const string expression2 = "(A + B ( + C ";
    const string expression3 = "(A + B + C ";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
    
    lexer.Tokenize(expression2);
    currentToken = begin(lexer.Tokens());
    endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
    
    lexer.Tokenize(expression3);
    currentToken = begin(lexer.Tokens());
    endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAdditionWithParenthesis3)
{
    const string expression = "(A (+) B)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAdditionWithParenthesis4)
{
    const string expression = "(A (+ B))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinaryAdditionWithParenthesis5)
{
    // This misses one close parenthesis after B
    const string expression = "(((((A))))) + ((((B))) + ((C + (((D))) + E))";
    // This misses one open parenthesis before D
    const string expression2 = "(((((A))))) + ((((B)))) + ((C + ((D))) + E))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());

    lexer.Tokenize(expression2);
    currentToken = begin(lexer.Tokens());
    endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtaction)
{
    const string expression = "A - B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtaction2)
{
    const string expression = "A - B - C";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}


TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtraction3)
{
    const string expression = "A - B - C - D";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtraction4)
{
    const string expression = "A - - B";
    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtraction2)
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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtraction3)
{
    const string expression = "A -";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtraction4)
{
    const string expression = "A - B - C - - -";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtractionWithParenthesis)
{
    const string expression = "(A - B)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtractionWithParenthesis2)
{
    const string expression = "(A - B - C)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtractionWithParenthesis3)
{
    const string expression = "(A - (B - C))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtractionWithParenthesis4)
{
    const string expression = "(A - ((B) - C))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinarySubtractionWithParenthesis5)
{
    const string expression = "(((((A))))) - ((((B)))) - ((C - (((D))) - E))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtractionWithParenthesis)
{
    const string expression = "(A - (() - C))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtractionWithParenthesis2)
{
    const string expression = "A - B ( - C )";
    const string expression2 = "A - B  - C)";
    const string expression3 = "A - B) - C) ";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
    
    lexer.Tokenize(expression2);
    currentToken = begin(lexer.Tokens());
    endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
    
    lexer.Tokenize(expression3);
    currentToken = begin(lexer.Tokens());
    endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtractionWithParenthesis3)
{
    const string expression = "(A (-) B)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtractionWithParenthesis4)
{
    const string expression = "(A (- B))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidBinarySubtractionWithParenthesis5)
{
    // This misses one close parenthesis after B
    const string expression = "(((((A))))) - ((((B))) - ((C - (((D))) - E))";
    // This misses one open parenthesis before D
    const string expression2 = "(((((A))))) - ((((B)))) - ((C - ((D))) - E))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());

    lexer.Tokenize(expression2);
    currentToken = begin(lexer.Tokens());
    endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryMultiplication)
{
    const string expression = "A * B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryMultiplication2)
{
    const string expression = "(A * (B))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryDivision)
{
    const string expression = "A / B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryDivision2)
{
    const string expression = "(A) / B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryBitwiseAnd)
{
    const string expression = "A & B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryBitwiseAnd2)
{
    const string expression = "(A & (B & D)) & E";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryBitwiseOr)
{
    const string expression = "A | B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryBitwiseOr2)
{
    const string expression = "A | B | (C | D) | E";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryBitwiseXor)
{
    const string expression = "A ^ B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryBitwiseXor2)
{
    const string expression = "A ^ B ^ (C ^ D) ^ E";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLeftShift)
{
    const string expression = "A << B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLeftShift2)
{
    const string expression = "A << (B << (C << D))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryRightShift)
{
    const string expression = "A >> B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryRightShift2)
{
    const string expression = "(A >> B) >> (C >> D)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryEquality)
{
    const string expression = "A == B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryEquality2)
{
    const string expression = "(A == (B == E)) == ((C == (D == F)) == N)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryDifference)
{
    const string expression = "A != B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryDifference2)
{
    const string expression = "A != B != C != D != (E != F != G != H)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLogicAnd)
{
    const string expression = "A && B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLogicAnd2)
{
    const string expression = "((((((A) && B) && C) && D) && E) && F)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLogicOr)
{
    const string expression = "A || B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLogicOr2)
{
    const string expression = "(A) || (B) || (C) || (D)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryGreaterThan)
{
    const string expression = "A > B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryGreaterThan2)
{
    const string expression = "(A > B) && (C > D)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLessThan)
{
    const string expression = "A < B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLessThan2)
{
    const string expression = "(A < F) || (A < D)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryGreaterThanOrEqualTo)
{
    const string expression = "A >= B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryGreaterThanOrEqualTo2)
{
    const string expression = "(A >= F) == (A >= D)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLessThanOrEqualTo)
{
    const string expression = "A <= B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryLessThanOrEqualTo2)
{
    const string expression = "(A <= (B)) != (A <= (D + E))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryThreeWay)
{
    const string expression = "A <=> B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryThreeWay2)
{
    const string expression = "((A <=> B)) && (A <=> D * E)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAssignment)
{
    const string expression = "A = B";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseBinaryAssigment2)
{
    const string expression = "A = (B + C * D - E / F)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryLogicNegation)
{
    const string expression = "!A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryLogicNegation2)
{
    const string expression = "!(A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryLogicNegation3)
{
    const string expression = "!(!A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryLogicNegation4)
{
    const string expression = "! !A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseInvalidUnaryOperation)
{
    const string expression = "A !";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryBitwiseNegation)
{
    const string expression = "~A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryBitwiseNegation2)
{
    const string expression = "~(A + ~B & ~(C))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryNegative)
{
    const string expression = "-A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryNegative2)
{
    const string expression = "-(A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryNegative3)
{
    const string expression = "(-A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryNegative4)
{
    const string expression = "(A + -A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryNegative5)
{
    const string expression = "(A +  - - - -A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryPositive)
{
    const string expression = "+A";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryPositive2)
{
    const string expression = "+(A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryPositive3)
{
    const string expression = "(+A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryPositive4)
{
    const string expression = "(A + +A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryPositive5)
{
    const string expression = "(A - + + + + + A)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

/*TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationHLCheck)
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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryGreaterThanOperationLexicalAnalysis)
{
    const string expression = "A > 0d55";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorGREATERTHAN, TokenType::LiteralNumericDECIMAL}; // A > 0d55

    auto lexemes = { "A", Lexemes::OperatorGREATERTHAN.c_str(), "0d55" }; // A > 0d55

    auto columns = { 1llu, 3llu, 5llu }; // A > 0d55
    
    auto lines = { 1llu, 1llu, 1llu }; // A > 0d55


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck11)
{
    const string expression = "A > F";

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
    EXPECT_STREQ("F", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ(">", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryGreaterThanOrEqualOperationLexicalAnalysis)
{
    const string expression = "A >= MY_VALUE";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorGREATERTHANOREQUALTO, TokenType::Identifier }; // A >= MY_VALUE

    auto lexemes = { "A", Lexemes::OperatorGREATERTHANOREQUALTO.c_str(), "MY_VALUE" }; // A >= MY_VALUE

    auto columns = { 1llu, 3llu, 6llu }; // A >= MY_VALUE
    
    auto lines = { 1llu, 1llu, 1llu }; // A >= MY_VALUE


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck12)
{
    const string expression = "SOME_VALUE >= 9";

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
    EXPECT_STREQ("9", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ(">=", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("SOME_VALUE", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryEqualOperationLexicalAnalysis)
{
    const string expression = "A == TRUE";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorEQUAL, TokenType::LiteralBooleanTRUE}; // A == TRUE

    auto lexemes = { "A", Lexemes::OperatorEQUAL.c_str(), "TRUE" }; // A == TRUE

    auto columns = { 1llu, 3llu, 6llu }; // A == TRUE
    
    auto lines = { 1llu, 1llu, 1llu }; // A == TRUE


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck13)
{
    const string expression = "MY_BOOLEAN == FALSE";

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
    EXPECT_STREQ("FALSE", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("==", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("MY_BOOLEAN", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryDifferentOperationLexicalAnalysis)
{
    const string expression = "A != \"FALSE\"";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorDIFFERENT, TokenType::LiteralSTRING}; // A != "FALSE"

    auto lexemes = { "A", Lexemes::OperatorDIFFERENT.c_str(), "\"FALSE\"" }; // A != "FALSE"

    auto columns = { 1llu, 3llu, 6llu }; // A != "FALSE"
    
    auto lines = { 1llu, 1llu, 1llu }; // A != "FALSE"


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck14)
{
    const string expression = "MY_CHAR != 'a'";

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
    EXPECT_STREQ("'a'", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("!=", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("MY_CHAR", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryBitwiseAndOperationLexicalAnalysis)
{
    const string expression = "A & 0x01";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorBITWISEAND, TokenType::LiteralNumericHEXADECIMAL}; // A & 0x01

    auto lexemes = { "A", Lexemes::OperatorBITWISEAND.c_str(), "0x01" }; // A & 0x01

    auto columns = { 1llu, 3llu, 5llu }; // A & 0x01
    
    auto lines = { 1llu, 1llu, 1llu }; // A & 0x01


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck15)
{
    const string expression = "A & 0b00101111";

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
    EXPECT_STREQ("0b00101111", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("&", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryBitwiseXorOperationLexicalAnalysis)
{
    const string expression = "A ^ B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorBITWISEXOR, TokenType::Identifier}; // A ^ B

    auto lexemes = { "A", Lexemes::OperatorBITWISEXOR.c_str(), "B" }; // A ^ B

    auto columns = { 1llu, 3llu, 5llu }; // A ^ B
    
    auto lines = { 1llu, 1llu, 1llu }; // A ^ B

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck16)
{
    const string expression = "A ^ 0xFF";

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
    EXPECT_STREQ("0xFF", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("^", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryBitwiseOrOperationLexicalAnalysis)
{
    const string expression = "A | B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorBITWISEOR, TokenType::Identifier}; // A | B

    auto lexemes = { "A", Lexemes::OperatorBITWISEOR.c_str(), "B" }; // A | B

    auto columns = { 1llu, 3llu, 5llu }; // A | B
    
    auto lines = { 1llu, 1llu, 1llu }; // A | B

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck17)
{
    const string expression = "A | 0o00";

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
    EXPECT_STREQ("0o00", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("|", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryLogicAndOperationLexicalAnalysis)
{
    const string expression = "A && B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorLOGICAND, TokenType::Identifier}; // A && B

    auto lexemes = { "A", Lexemes::OperatorLOGICAND.c_str(), "B" }; // A && B

    auto columns = { 1llu, 3llu, 6llu }; // A && B
    
    auto lines = { 1llu, 1llu, 1llu }; // A && B

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck18)
{
    const string expression = "A && MY_CONDITION";

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
    EXPECT_STREQ("MY_CONDITION", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("&&", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryLogicOrOperationLexicalAnalysis)
{
    const string expression = "A || B";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorLOGICOR, TokenType::Identifier}; // A || B

    auto lexemes = { "A", Lexemes::OperatorLOGICOR.c_str(), "B" }; // A || B

    auto columns = { 1llu, 3llu, 6llu }; // A || B
    
    auto lines = { 1llu, 1llu, 1llu }; // A || B

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck19)
{
    const string expression = "A || TRUE";

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
    EXPECT_STREQ("TRUE", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("||", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryAssignmentOperationLexicalAnalysis)
{
    const string expression = "A = \"Test\"";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorASSIGNMENT, TokenType::LiteralSTRING}; // A = "Test"

    auto lexemes = { "A", Lexemes::OperatorASSIGNMENT.c_str(), "\"Test\"" }; // A = "Test"

    auto columns = { 1llu, 3llu, 5llu }; // A = "Test"
    
    auto lines = { 1llu, 1llu, 1llu }; // A = "Test"

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck20)
{
    const string expression = "A = 1";

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
    EXPECT_STREQ("1", representation.Lexeme.c_str());
    stack.pop();

    representation = stack.top();
    EXPECT_STREQ("=", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("A", representation.Lexeme.c_str());
    stack.pop();
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, SanityCheckExpressionBinaryOperatorWithPackMember)
{
    const string expression = "A = MyPack.MyMember";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorASSIGNMENT, TokenType::Identifier, TokenType::OperatorDOT, TokenType::Identifier}; // A = MyPack.MyMember

    auto lexemes = { "A", Lexemes::OperatorASSIGNMENT.c_str(), "MyPack", Lexemes::OperatorDOT.c_str(), "MyMember" }; // A = MyPack.MyMember

    auto columns = { 1llu, 3llu, 5llu, 11llu, 12llu }; // A = MyPack.MyMember
    
    auto lines = { 1llu, 1llu, 1llu, 1llu, 1llu }; // A = MyPack.MyMember

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseSimpleBinaryExpressionIntermediateRepresentationLLCheck21)
{
    const string expression = "Pack.Field = A";

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
    EXPECT_STREQ("=", representation.Lexeme.c_str());
    stack.pop();
    
    representation = stack.top();
    EXPECT_STREQ("Pack.Field", representation.Lexeme.c_str());
    stack.pop();
}*/