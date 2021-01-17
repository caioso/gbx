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
    FAIL() << "exception '" << MESSAGE << "' not thrown";             \
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
                               "B'01010100, b'111111111, 0B00000000, 0b10101010101010101, "
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
                               "B'0010.1110, b'1111.0000.1111.01010, 0B00001101, 0b111000010001.1111, 0b11110000000011110000, "
                               "1234";

    const auto baseList = 
    {
        NumericBase::Hexadecimal, NumericBase::Hexadecimal, NumericBase::Hexadecimal, NumericBase::Hexadecimal,
        NumericBase::Decimal, NumericBase::Decimal, NumericBase::Decimal, NumericBase::Decimal, 
        NumericBase::Octal, NumericBase::Octal, NumericBase::Octal, NumericBase::Octal,
        NumericBase::Binary, NumericBase::Binary, NumericBase::Binary, NumericBase::Binary, NumericBase::Binary,       
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

TEST(TestNumberParser, CheckForInvalidNumericBasesCharacters)
{
    const auto stringContant = "H'hOhH, h'LLhoH, 0XHLooXX, 0x1112XHL, "
                               "D'AO9, d'12FF, 0D1234h, 0d12d.34, "
                               "O'9AFF, o'8Xo1, 0O0008, 0olabcc9, "
                               "B'12345678.abcde, b'0000000000012, 0B91881.9991, 0b67111AAdfeeeeee.e11110, "
                               "1234FFE 1234.11123";

    const std::string exceptionMessageList[] = 
    {
        static_cast<string>("Invalid hexadecimal number 'H'hOhH'"),
        static_cast<string>("Invalid hexadecimal number 'h'LLhoH'"),
        static_cast<string>("Invalid hexadecimal number '0XHLooXX'"),
        static_cast<string>("Invalid hexadecimal number '0x1112XHL'"),
        static_cast<string>("Invalid decimal number 'D'AO9'"),
        static_cast<string>("Invalid decimal number 'd'12FF'"),
        static_cast<string>("Invalid decimal number '0D1234h'"),
        static_cast<string>("Invalid decimal number '0d12d.34'"),
        static_cast<string>("Invalid octal number 'O'9AFF'"),
        static_cast<string>("Invalid octal number 'o'8Xo1'"),
        static_cast<string>("Invalid octal number '0O0008'"),
        static_cast<string>("Invalid octal number '0olabcc9'"),
        static_cast<string>("Invalid binary number 'B'12345678.abcde'"),
        static_cast<string>("Invalid binary number 'b'0000000000012'"),
        static_cast<string>("Invalid binary number '0B91881.9991'"),
        static_cast<string>("Invalid binary number '0b67111AAdfeeeeee.e11110'"),
        static_cast<string>("Invalid decimal number '1234FFE'"),
        static_cast<string>("Invalid decimal number '1234.11123'"),
    };

    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
    auto numberParser = make_shared<ParsedNumber>();
    auto counter = 0;

    tokenizer->ToToken(numberString);

    for (auto token : tokenizer->Tokens())
    {
       ASSERT_EXCEPTION( { numberParser->TryParse(token); }, 
                          ParsedNumberException, 
                          exceptionMessageList[counter++].c_str());    
    }
}

TEST(TestNumberParser, TestNumericValues)
{
    const auto stringContant = "0xFFFF, H'1234, h'Aa, 0X00D1";
    auto tokenizer = make_shared<Tokenizer>();
    auto parsedNumber = make_shared<ParsedNumber>();
    tokenizer->ToToken(stringContant);

    parsedNumber->TryParse(tokenizer->Tokens()[0]);
    EXPECT_EQ(static_cast<uint16_t>(0xFFFF), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[1]);
    EXPECT_EQ(static_cast<uint16_t>(0x1234), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[2]);
    EXPECT_EQ(static_cast<uint16_t>(0x00Aa), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[3]);
    EXPECT_EQ(static_cast<uint16_t>(0x00D1), static_cast<uint16_t>(parsedNumber->Value()));
}
