#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "../src/Tokenizer.h"

using namespace gbxasm;
using namespace std;

class TokenizerDecorator : public Tokenizer
{
public:
    string GetInput() { return _input; }
};

TEST(TestTokenizer, Construction)
{
    auto tokenizer = make_shared<Tokenizer>();
}

TEST(TestTokenizer, SetTokenizerInput)
{
    const auto testString = static_cast<string>("test string to be tokenized");
    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    EXPECT_EQ(testString, tokenizer->GetInput());
}

TEST(TestTokenizer, TestTokenExtractionSimpleText)
{
    const auto testString = static_cast<string>("test string to be tokenized");
    const auto tokenList = 
    { 
        static_cast<string>("test"),
        static_cast<string>("string"),
        static_cast<string>("to"),
        static_cast<string>("be"),
        static_cast<string>("tokenized")
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
        EXPECT_EQ(*(begin(tokenList) + counter++), token.Token);
}

TEST(TestTokenizer, TestTokenListExtraSpace)
{
    const auto testString = static_cast<string>("        when       you        walk               away      you                 don't                hear   me say      PLEASE   ");
    const auto tokenList = 
    { 
        static_cast<string>("when"),
        static_cast<string>("you"),
        static_cast<string>("walk"),
        static_cast<string>("away"),
        static_cast<string>("you"),
        static_cast<string>("don't"),
        static_cast<string>("hear"),
        static_cast<string>("me"),
        static_cast<string>("say"),
        static_cast<string>("PLEASE"),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
        EXPECT_EQ(*(begin(tokenList) + counter++), token.Token);
}

TEST(TestTokenizer, TestTokenListSpecialSpaces)
{
    const auto testString = static_cast<string>("\tthis\ntext\nhas\tstrange\n\t\rformatting\r...");
    const auto tokenList = 
    { 
        static_cast<string>("this"),
        static_cast<string>("text"),
        static_cast<string>("has"),
        static_cast<string>("strange"),
        static_cast<string>("formatting"),
        static_cast<string>("..."),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
        EXPECT_EQ(*(begin(tokenList) + counter++), token.Token);
}

TEST(TestTokenizer, CheckTokensLineNumber)
{
    const auto testString = static_cast<string>("\tthis\ntext\n\nhas\tstrange\n\t\rformatting\r...");
    auto tokenList = 
    { 
        TokenMaker::MakeToken("this", 1, 2),
        TokenMaker::MakeToken("text", 2, 1),
        TokenMaker::MakeToken("has", 4, 1),
        TokenMaker::MakeToken("strange", 4, 5),
        TokenMaker::MakeToken("formatting", 5, 3),
        TokenMaker::MakeToken("...", 5, 14),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
    {
        EXPECT_EQ((*(begin(tokenList) + counter)).Token, token.Token);
        EXPECT_EQ((*(begin(tokenList) + counter)).Line, token.Line);
        EXPECT_EQ((*(begin(tokenList) + counter++)).Column, token.Column);
    }
}

TEST(TestTokenizer, CheckTokensLineNumberWithSpaces)
{
    const auto testString = static_cast<string>("\tthis          \n    text        \n   \n    has               \t \r\t\tstrange  \t\t\r\t\t         \n\t\r     formatting         \t\r           ... \t\t\t\r");
    auto tokenList = 
    { 
        TokenMaker::MakeToken("this", 1, 2),
        TokenMaker::MakeToken("text", 2, 5),
        TokenMaker::MakeToken("has", 4, 5),
        TokenMaker::MakeToken("strange", 4, 28),
        TokenMaker::MakeToken("formatting", 5, 8),
        TokenMaker::MakeToken("...", 5, 40),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
    {
        EXPECT_EQ((*(begin(tokenList) + counter)).Token, token.Token);
        EXPECT_EQ((*(begin(tokenList) + counter)).Line, token.Line);
        EXPECT_EQ((*(begin(tokenList) + counter++)).Column, token.Column);
    }
}

TEST(TestTokenizer, DetectNumericTokens)
{
    const auto testString = static_cast<string>("0x1143 H'1245 Ob1111.0000 5412 H'4511.L");
    auto tokenList = 
    { 
        TokenMaker::MakeToken("0x1143", 1, 1),
        TokenMaker::MakeToken("H'1245", 1, 8),
        TokenMaker::MakeToken("Ob1111.0000", 1, 15),
        TokenMaker::MakeToken("5412", 1, 27),
        TokenMaker::MakeToken("H'4511.L", 1, 32),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
    {
        EXPECT_EQ((*(begin(tokenList) + counter)).Token, token.Token);
        EXPECT_EQ((*(begin(tokenList) + counter)).Line, token.Line);
        EXPECT_EQ((*(begin(tokenList) + counter++)).Column, token.Column);
    }
}

TEST(TestTokenizer, TestDelimiterElimination)
{
    const auto testString = static_cast<string>("Token11, Token2; ;Token3 ,Token4 ,Token5, ;Token6, ,Token7; ;Token8;");
    auto tokenList = 
    { 
        TokenMaker::MakeToken("Token11,", 1, 1),
        TokenMaker::MakeToken("Token2;", 1, 10),
        TokenMaker::MakeToken(";Token3", 1, 18),
        TokenMaker::MakeToken(",Token4", 1, 26),
        TokenMaker::MakeToken(",Token5,", 1, 34),
        TokenMaker::MakeToken(";Token6,", 1, 43),
        TokenMaker::MakeToken(",Token7;", 1, 52),
        TokenMaker::MakeToken(";Token8;", 1, 61),
    };

    auto tokenListWithoutDelimiters = 
    { 
        TokenMaker::MakeToken("Token11", 1, 1),
        TokenMaker::MakeToken("Token2", 1, 10),
        TokenMaker::MakeToken("Token3", 1, 18),
        TokenMaker::MakeToken("Token4", 1, 26),
        TokenMaker::MakeToken("Token5", 1, 34),
        TokenMaker::MakeToken("Token6", 1, 43),
        TokenMaker::MakeToken("Token7", 1, 52),
        TokenMaker::MakeToken("Token8", 1, 61),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
    {
        EXPECT_EQ((*(begin(tokenList) + counter)).Token, token.Token);
        EXPECT_EQ((*(begin(tokenListWithoutDelimiters) + counter)).Token, token.TokenWithoutDelimiter);
        EXPECT_EQ((*(begin(tokenList) + counter)).Line, token.Line);
        EXPECT_EQ((*(begin(tokenList) + counter++)).Column, token.Column);
    }
}

TEST(TestTokenizer, TestMidWordDelimiter)
{
    const auto testString = static_cast<string>("Token.token,token2, token,1,token; ,token.,1;23token ");
    auto tokenList = 
    { 
        TokenMaker::MakeToken("Token.token,token2,", 1, 1),
        TokenMaker::MakeToken("token,1,token;", 1, 21),
        TokenMaker::MakeToken(",token.,1;23token", 1, 36),
    };

    auto tokenListWithoutDelimiters = 
    { 
        TokenMaker::MakeToken("Token.token,token2", 1, 1),
        TokenMaker::MakeToken("token,1,token", 1, 21),
        TokenMaker::MakeToken("token.,1;23token", 1, 36),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
    {
        EXPECT_EQ((*(begin(tokenList) + counter)).Token, token.Token);
        EXPECT_EQ((*(begin(tokenListWithoutDelimiters) + counter)).Token, token.TokenWithoutDelimiter);
        EXPECT_EQ((*(begin(tokenList) + counter)).Line, token.Line);
        EXPECT_EQ((*(begin(tokenList) + counter++)).Column, token.Column);
    }
}