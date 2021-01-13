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
        MakeToken("this", 1),
        MakeToken("text", 2),
        MakeToken("has", 4),
        MakeToken("strange", 4),
        MakeToken("formatting", 5),
        MakeToken("...", 5),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
    {
        EXPECT_EQ((*(begin(tokenList) + counter)).Token, token.Token);
        EXPECT_EQ((*(begin(tokenList) + counter++)).Line, token.Line);
    }
}


TEST(TestTokenizer, CheckTokensLineNumberWithSpaces)
{
    const auto testString = static_cast<string>("\tthis          \n    text        \n   \n    has               \t \r\t\tstrange  \t\t\r\t\t         \n\t\r     formatting         \t\r           ... \t\t\t\r");
    auto tokenList = 
    { 
        MakeToken("this", 1),
        MakeToken("text", 2),
        MakeToken("has", 4),
        MakeToken("strange", 4),
        MakeToken("formatting", 5),
        MakeToken("...", 5),
    };

    auto tokenizer = make_shared<TokenizerDecorator>();
    tokenizer->ToToken(testString);

    auto extractedTokens = tokenizer->Tokens();

    auto counter = 0;
    for (auto token : extractedTokens)
    {
        EXPECT_EQ((*(begin(tokenList) + counter)).Token, token.Token);
        EXPECT_EQ((*(begin(tokenList) + counter++)).Line, token.Line);
    }
}