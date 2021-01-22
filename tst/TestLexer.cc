#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "../src/Lexer.h"
#include "../src/Lexemes.h"

using namespace gbxasm;
using namespace std;

TEST(TestLexer, Construction)
{
    auto lexer = make_shared<Lexer>();
}

TEST(TestLexer, AcquireReferenceOfTokensVector)
{
    auto lexer = make_shared<Lexer>();
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(0), tokens.size());
}

TEST(TestLexer, BasicTokenization)
{
    const string program = "";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(0), tokens.size());
}

TEST(TestLexer, BasicKeywordTokenization)
{
    const string program = "PACK";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPack.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateBasicLexeme)
{
    const string program = "PACK";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPack.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateLexemeWithOperator)
{
    const string program = "PACK+";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPack.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);

    EXPECT_STREQ(Lexemes::OperatorPlus.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[1].Column);
}
