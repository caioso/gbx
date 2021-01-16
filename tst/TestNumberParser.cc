#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "../src/GBXAsmExceptions.h"
#include "../src/Token.h"
#include "../src/Tokenizer.h"
#include "../src/symbols/Number.h"
#include "../src/utils/ParsedNumber.h"

#define ASSERT_EXCEPTION( TRY_BLOCK, EXCEPTION_TYPE, MESSAGE )        \
try                                                                   \
{                                                                     \
    TRY_BLOCK                                                         \
    FAIL() << "exception '" << MESSAGE << "' not thrown at all!";     \
}                                                                     \
catch( const EXCEPTION_TYPE& e )                                      \
{                                                                     \
    EXPECT_STREQ( MESSAGE, e.what() )                                    \
        << " exception message is incorrect. Expected the following " \
           "message:\n\n"                                             \
        << e.what() << "\n";                                           \
}                                                                     \
catch( ... )                                                          \
{                                                                     \
    FAIL() << "exception '" << MESSAGE                                \
           << "' not thrown with expected type '" << #EXCEPTION_TYPE  \
           << "'!";                                                   \
}

using namespace gbxasm;
using namespace gbxasm::utils;
using namespace std;

TEST(TestNumberParser, ParseDecimalNumbers)
{
    auto numberString = static_cast<string>("    1234   ");
    auto tokenizer = make_shared<Tokenizer>();
    auto numberParser = make_shared<ParsedNumber>();

    tokenizer->ToToken(numberString);

    for (auto token : tokenizer->Tokens())
    {
        numberParser->TryParse(token);
        EXPECT_EQ(static_cast<uint32_t>(1234), numberParser->Value());
        EXPECT_EQ(NumericBase::Decimal, numberParser->Base());
    }
}

TEST(TestNumberParser, ParseInvalidDecimalNumbers)
{
    auto numberString = static_cast<string>("    &1234   ");
    auto tokenizer = make_shared<Tokenizer>();
    auto numberParser = make_shared<ParsedNumber>();

    tokenizer->ToToken(numberString);

    for (auto token : tokenizer->Tokens())
    {
        ASSERT_EXCEPTION( { numberParser->TryParse(token); }, 
                          ParsedNumberException, 
                          "Invalid character '&' in numeric constant '&1234'");
        
    }
}

TEST(TestNumberParser, ParserEvaluatePrefixes)
{
    const auto stringContant = "H'FFFF, h'ffff, 0XFFFF, 0xffff, "
                               "D'1234, d'1234, 0D1234, 0d1234, "
                               "O'1234, o'1234, 0O1234, 0o1234, "
                               "B'1234, b'1234, 0B1234, 0b1234, "
                               "1234";
    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
    auto numberParser = make_shared<ParsedNumber>();

    tokenizer->ToToken(numberString);

    for (auto token : tokenizer->Tokens())
    {
        try
        {
            numberParser->TryParse(token);
        }
        catch (const ParsedNumberException& e)
        {
            cout << "Exception Message: " << e.what() << '\n';
            FAIL();
        }
        
    }
}

TEST(TestNumberParser, CheckParsedPrefixes)
{
    const auto stringContant = "H'FFFF, h'ffff, 0XFFFF, 0xffff, "
                               "D'1234, d'1234, 0D1234, 0d1234, "
                               "O'1234, o'1234, 0O1234, 0o1234, "
                               "B'1234, b'1234, 0B1234, 0b1234, "
                               "1234";

    const auto baseList = 
    {
        NumericBase::Hexadecimal, NumericBase::Hexadecimal, NumericBase::Hexadecimal, NumericBase::Hexadecimal,
        NumericBase::Decimal, NumericBase::Decimal, NumericBase::Decimal, NumericBase::Decimal, 
        NumericBase::Octal, NumericBase::Octal, NumericBase::Octal, NumericBase::Octal,
        NumericBase::Binary, NumericBase::Binary, NumericBase::Binary, NumericBase::Binary,    
        NumericBase::Decimal, 
    };

    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
    auto numberParser = make_shared<ParsedNumber>();
    auto counter = 0;

    tokenizer->ToToken(numberString);

    for (auto token : tokenizer->Tokens())
    {
       numberParser->TryParse(token);

       EXPECT_EQ(*(begin(baseList) + counter++), numberParser->Base());        
    }
}
