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

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithNumericLiteral)
{
    const string expression = "0xFF + 0x01";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithNumericLiteral2)
{
    const string expression = "MY_RESULT = (-0o31 + 150)/(FIRST_CONSTANT * (40*SECOND_CONSTANT))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithNumericLiteral3)
{
    const string expression = "COMPARISON = (((0b11110000 | 0b00001111) == 0xFF))";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithStringLiteral)
{
    const string expression = "\"Hello\" + \" \" + \"World!\"";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithStringLiteral2)
{
    const string expression = "MY_STRING = \"GAMEBOY\" + \"\" + \"X\"";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithCharLiteral)
{
    const string expression = "'G' + 'B' + 'X'";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithCharLiteral2)
{
    const string expression = "MY_TEST = 'H' + 'E' + 'L' + 'L' + 'O'";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithBooleanLiteral)
{
    const string expression = "MY_TEST = TRUE | FALSE";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithBooleanLiteral2)
{
    const string expression = "(A == 0xFF) == TRUE || (B != 0xFE) == TRUE";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPackIdentifier)
{
    const string expression = "A.B + C.D";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPackIdentifier2)
{
    const string expression = "SPRITE.X = (SCREEN_ORIGIN_X + 0xFF) * 45";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPackIdentifier3)
{
    const string expression = "(MY_REG.VALUE < CONSTANTS.VALUE == TRUE)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithInvalidPackIdentifier)
{
    const string expression = ".INVALID + 1";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    ASSERT_EXCEPTION( { parser.TryToAccept(currentToken, endIterator); }, 
                      SyntacticAnalyzerException, 
                      "Unexpected '.' found");
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithInvalidPackIdentifier2)
{
    const string expression = "CONSTANT + INVALID.";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    ASSERT_EXCEPTION( { parser.TryToAccept(currentToken, endIterator); }, 
                      SyntacticAnalyzerException, 
                      "Unexpected '.' found");
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithInvalidPackIdentifier3)
{
    const string expression = "(INVALID. + 1)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    ASSERT_EXCEPTION( { parser.TryToAccept(currentToken, endIterator); }, 
                      SyntacticAnalyzerException, 
                      "Expected identifier after '.'");
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithInvalidPackIdentifier4)
{
    const string expression = "(.INVALID + 1)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    ASSERT_EXCEPTION( { parser.TryToAccept(currentToken, endIterator); }, 
                      SyntacticAnalyzerException, 
                      "Expected identifier before '.'");
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation)
{
    const string expression = "A + BOB";

    // Expected Result  
    //              EXP2 Binary(EXP0 + EXP1)
    //               |
    //              / \
    //             /   \
    //          EXP0   EXP1
    // Resolve_ID(A)   Resolve_ID(B)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x03llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x08llu, expressionRepresentation->EndColumn());
    
    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("BOB", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation2)
{
    const string expression = "A + B - C";

    // Expected Result  
    //                          EXP4 Binary(EXP2 - EXP3)
    //                           |
    //                          / \
    //                         /   \
    //  Binary(EXP0 + EXP1) EXP2   EXP3
    //                       |     Resolve_ID(C)
    //                       |
    //                      / \
    //                     /   \
    //                 EXP0    EXP1
    //        Resolve_ID(A)    Resolve_ID(B)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x05llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x0Allu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Operator::BinarySubtraction, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("C", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation3)
{
    const string expression = "(A + B)";

    // Expected Result  
    //              EXP2 Binary(EXP0 + EXP1)
    //               |
    //              / \
    //             /   \
    //          EXP0   EXP1
    // Resolve_ID(A)   Resolve_ID(B)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x03llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x08llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation4)
{
    const string expression = "(A + B) * C";

    // Expected Result  
    //                          EXP4 Binary(EXP2 + EXP3)
    //                           |
    //                          / \
    //                         /   \
    //  Binary(EXP0 + EXP1) EXP2   EXP3
    //                       |     Resolve_ID(C)
    //                       |
    //                      / \
    //                     /   \
    //                 EXP0    EXP1
    //        Resolve_ID(A)    Resolve_ID(B)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x05llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x0Cllu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryMultiplication, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("C", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());     
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation5)
{
    const string expression = "(A + (B + C))";

    // Expected Result  
    //                          EXP4 Binary(EXP0 + EXP3)
    //                           |
    //                          / \
    //                         /   \
    //                      EXP0   EXP3 Binary(EXP1 + EXP2)
    //             Resolve_ID(A)    |     
    //                              |
    //                             / \
    //                            /   \
    //                        EXP1    EXP2
    //               Resolve_ID(B)    Resolve_ID(C)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x05llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x0Ellu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("C", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation6)
{
    const string expression = "(A / (B - C)) * D";

    // Expected Result  
    //                             EXP6 Binary(EXP4 * EXP5)
    //                              |
    //                             / \
    //                            /   \
    //     Binary(EXP0 / EXP3) EXP4   EXP5
    //                           |    Resolve_ID(D)
    //                          / \
    //                         /   \
    //                      EXP0   EXP3 Binary(EXP1 - EXP2)
    //             Resolve_ID(A)    |     
    //                              |
    //                             / \
    //                            /   \
    //                        EXP1    EXP2
    //               Resolve_ID(B)    Resolve_ID(C)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x07lu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x12llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(6llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryMultiplication, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp4", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp5", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(5llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("D", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
    member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Operator::BinaryDivision, member.OperatorType);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinarySubtraction, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("C", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation7)
{
    const string expression = "!A";

    // Expected Result  
    //              EXP1 Unary(EXP0)
    //               |
    //              EXP0
    //              Resolve_ID(A)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x02llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x03llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryLogicNegation, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation8)
{
    const string expression = "~(B)";

    // Expected Result  
    //              EXP1 Unary(EXP0)
    //               |
    //              EXP0
    //              Resolve_ID(B)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x02llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x05llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryBitwiseNegation, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation9)
{
    const string expression = "-(A & B)";

    // Expected Result  
    //                EXP3 Unary(-EXP2)
    //                 |
    //                EXP2 Binary(EXP0 & EXP1)
    //                 |
    //                / \
    //               /   \
    //           EXP0    EXP1
    //  Resolve_ID(A)    Resolve_ID(B)
    //
    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x04llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x09llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Operator::UnaryNegative, member.OperatorType);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryBitwiseAnd, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation10)
{
    const string expression = "(-A + !(B + ~C))";

    // Expected Result  
    //                         EXP7 Binary(EXP1 & EXP6)
    //                          |
    //                         / \
    //                        /   \
    //                       /     \
    //       Unary(-EXP0) EXP1     EXP6 Unary(!EXP5)
    //                     |        |
    //                    EXP0    EXP5 Binary(EXP2 + EXP4)
    //           Resolve_ID(A)      |
    //                             / \
    //                            /   \
    //                          EXP2  EXP4 Unary(~EXP3)
    //                Resolve_ID(B)    |
    //                                EXP3
    //                                Resolve_ID(C)
    
    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x08llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x11llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(7llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp6", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(6llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryLogicNegation, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp5", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(5llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp4", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryBitwiseNegation, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp3", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("C", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("B", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryNegative, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryImmadiate)
{
    const string expression = "#0x1234";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseUnaryImmadiate2)
{
    const string expression = "#(ALIAS + VALUE)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}


TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation11)
{
    const string expression = "#(ALIAS + CONSTANT)";

    // Expected Result  
    //                    EXP3 Unary(#EXP2)
    //                     |
    //                    EXP2 Binary(EXP0 + EXP1)
    //                     |
    //                    / \
    //                   /   \
    //               EXP0    EXP1
    //  Resolve_ID(ALIAS)    Resolve_ID(CONSTANT)
    //
    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x04llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x14llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryImmediate, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("CONSTANT", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("ALIAS", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation12)
{
    const string expression = "[A]";

    // Expected Result  
    //              EXP0 Resolve_ID(A)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x01llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x04llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();
    
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::Memory, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation13)
{
    const string expression = "(Z + ![A])";

    // Expected Result  
    //                    EXP2 Binary(EXP0 + EXP2)
    //                     |
    //                    / \
    //                   /   \
    //               EXP0    EXP2 Unary (![EXP1])
    //      Resolve_ID(Z)     |
    //                        |
    //                       EXP1 Resolve_ID(A)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x04llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x0Bllu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();

    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp2", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryLogicNegation, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::Memory, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("A", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("Z", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
      
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPostIncrement)
{
    // Post-increment is only valid for Identifiers
    // Syntax -> IDENTIFIER++/IDENTIFIER--
    const string expression = "HL++";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPostIncrement2)
{
    // Post-increment is only valid for Identifiers
    // Syntax -> IDENTIFIER++/IDENTIFIER--
    const string expression = "[HL++] + (A + B + C)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation14)
{
    const string expression = "(Z * ([HL++] + 0x03))";

    // Expected Result  
    //                    EXP5 Binary(EXP0 * EXP4)
    //                     |
    //                    / \
    //                   /   \
    //               EXP0    EXP4 Binary(EXP2 + EXP3)
    //      Resolve_ID(Z)     |
    //                       / \
    //                      /   \
    //      Unary(EXP1++) EXP2  EXP3
    //                     |    Resolve_ID(0x03)
    //                    EXP1 
    //          Resolve_ID(HL)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x06llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x16llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();

    EXPECT_EQ(5llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryMultiplication, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp4", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::NumericLiteral, member.Operand1Type);
    EXPECT_STREQ("0x03", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::Memory, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryPostIncrement, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(3llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("HL", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("Z", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPreIncrement)
{
    // Pre-increment is only valid for Identifiers in GBX Mode
    // Syntax -> ++IDENTIFIER/--IDENTIFIER
    const string expression = "++HL";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPreIncrement2)
{
    // Pre-increment is only valid for Identifiers in GBX Mode
    // Syntax -> IDENTIFIER++/IDENTIFIER--
    const string expression = "[++HL] + (A + B + C)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation15)
{
    const string expression = "(Z * ([++HL] + 0x03))";

    // Expected Result  
    //                    EXP5 Binary(EXP0 * EXP4)
    //                     |
    //                    / \
    //                   /   \
    //               EXP0    EXP4 Binary(EXP2 + EXP3)
    //      Resolve_ID(Z)     |
    //                       / \
    //                      /   \
    //      Unary(++EXP1) EXP2  EXP3
    //                     |    Resolve_ID(0x03)
    //                    EXP1 
    //          Resolve_ID(HL)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x06llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x16llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();

    EXPECT_EQ(5llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryMultiplication, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp4", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::NumericLiteral, member.Operand1Type);
    EXPECT_STREQ("0x03", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::Memory, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryPreIncrement, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(3llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("HL", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("Z", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPostDecrement)
{
    // Post-increment is only valid for Identifiers
    // Syntax -> IDENTIFIER++/IDENTIFIER--
    const string expression = "HL--";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPostDecrement2)
{
    // Post-increment is only valid for Identifiers
    // Syntax -> IDENTIFIER++/IDENTIFIER--
    const string expression = "[HL--] - (PI/10)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation16)
{
    const string expression = "(Z * ([HL--] + 0x03))";

    // Expected Result  
    //                    EXP5 Binary(EXP0 * EXP4)
    //                     |
    //                    / \
    //                   /   \
    //               EXP0    EXP4 Binary(EXP2 + EXP3)
    //      Resolve_ID(Z)     |
    //                       / \
    //                      /   \
    //      Unary(EXP1--) EXP2  EXP3
    //                     |    Resolve_ID(0x03)
    //                    EXP1 
    //          Resolve_ID(HL)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x06llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x16llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();

    EXPECT_EQ(5llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryMultiplication, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp4", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::NumericLiteral, member.Operand1Type);
    EXPECT_STREQ("0x03", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::Memory, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryPostDecrement, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(3llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("HL", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("Z", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPreDecrement)
{
    // Pre-increment is only valid for Identifiers in GBX Mode
    // Syntax -> ++IDENTIFIER/--IDENTIFIER
    const string expression = "--HL";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, ParseExpressionWithPreDecrement2)
{
    // Pre-increment is only valid for Identifiers in GBX Mode
    // Syntax -> IDENTIFIER++/IDENTIFIER--
    const string expression = "[--HL] + (A + B + C)";

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_ExpressionSyntacticAnalysis, AnalyseExpressionIntermediateRepresentation17)
{
    const string expression = "(Z * ([--HL] + 0x03))";

    // Expected Result  
    //                    EXP5 Binary(EXP0 * EXP4)
    //                     |
    //                    / \
    //                   /   \
    //               EXP0    EXP4 Binary(EXP2 + EXP3)
    //      Resolve_ID(Z)     |
    //                       / \
    //                      /   \
    //      Unary(--EXP1) EXP2  EXP3
    //                     |    Resolve_ID(0x03)
    //                    EXP1 
    //          Resolve_ID(HL)

    LexicalAnalyzer lexer;
    ExpressionSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<ExpressionIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ(0x06llu, expressionRepresentation->ExpressionStack().size());

    EXPECT_EQ(0x01llu, expressionRepresentation->StartLine());
    EXPECT_EQ(0x01llu, expressionRepresentation->StartColumn());
    EXPECT_EQ(0x01llu, expressionRepresentation->EndLine());
    EXPECT_EQ(0x16llu, expressionRepresentation->EndColumn());

    auto stack = expressionRepresentation->ExpressionStack();
    auto member = stack.top();
    stack.pop();

    EXPECT_EQ(5llu, member.ExpressionID);
    EXPECT_EQ(0llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryMultiplication, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp0", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp4", member.Operand2.c_str());

    member = stack.top();
    stack.pop();
    EXPECT_EQ(4llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::Binary, member.Type);
    EXPECT_EQ(Surrounder::Expression, member.SurrounderType);
    EXPECT_EQ(Operator::BinaryAddition, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp2", member.Operand1.c_str());
    EXPECT_EQ(OperandType::Expression, member.Operand2Type);
    EXPECT_STREQ("exp3", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(3llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::NumericLiteral, member.Operand1Type);
    EXPECT_STREQ("0x03", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(2llu, member.ExpressionID);
    EXPECT_EQ(2llu, member.Depth);
    EXPECT_EQ(ExpressionType::Unary, member.Type);
    EXPECT_EQ(Surrounder::Memory, member.SurrounderType);
    EXPECT_EQ(Operator::UnaryPreDecrement, member.OperatorType);
    EXPECT_EQ(OperandType::Expression, member.Operand1Type);
    EXPECT_STREQ("exp1", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(1llu, member.ExpressionID);
    EXPECT_EQ(3llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("HL", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
    
    member = stack.top();
    stack.pop();
    EXPECT_EQ(0llu, member.ExpressionID);
    EXPECT_EQ(1llu, member.Depth);
    EXPECT_EQ(ExpressionType::ResolveOperand, member.Type);
    EXPECT_EQ(Surrounder::None, member.SurrounderType);
    EXPECT_EQ(Operator::NoOperator, member.OperatorType);
    EXPECT_EQ(OperandType::Identifier, member.Operand1Type);
    EXPECT_STREQ("Z", member.Operand1.c_str());
    EXPECT_EQ(OperandType::NoOperand, member.Operand2Type);
    EXPECT_STREQ("", member.Operand2.c_str());  
}
// Implement addressing mode surrounders
/*
Register -> register as operand
R, PP
Immediate -> Static 8/16 bit value as operand (expressions also supported)
#
indirect -> Content of address held by register/immediate address/expression is the operand
[ ]
-> Indexde is directly implemented with 
[REG + 0x01/EXP]
-> Implement Post Inc/Dec operators for indirect auto-increment/decrement modes
[HL++]
[HL--]
// Check for implementation of pre-increment in GBX MODE only
[++HL]
[--HL]
*/


