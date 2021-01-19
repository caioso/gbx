#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <array>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <sstream>
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
    EXPECT_STREQ( MESSAGE, e.what() )                                 \
        << " exception message is incorrect. Expected the following " \
           "message:\n\n"                                             \
        << e.what() << "\n";                                          \
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

string ToBinary(uint16_t number)
{
    auto returnString = string("");
    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> willHavePoint {0, 255};

    for (auto i = 15; i >= 0; --i)
    {
        returnString += ((number>>i) & 0x01) == 0? '0' : '1';

        if (i != 15 && i != 0 && willHavePoint(engine) < 100)
            returnString += '.';
    }
 
    return returnString;
}

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

TEST(TestNumberParser, ParserEvaluateSufixes)
{
    const auto stringContant = "H'ABCD, H'FF00:H, H'FF00:L, H'AADD:h, H'DDFF:l, "
                               "H'0110:high, H'1AAD:HIGH, H'1AAD:LOW, H'2033:low, H'1223:b[1], H'5541:B[0], "
                               "H'112A:bit[10], H'0001:BIT[0]";

    ModifierType modiferList[] = 
    {
        ModifierType::NoModifier, ModifierType::HigherHalf, ModifierType::LowerHalf, ModifierType::HigherHalf, ModifierType::LowerHalf,
        ModifierType::HigherHalf, ModifierType::HigherHalf, ModifierType::LowerHalf, ModifierType::LowerHalf,
        ModifierType::Bit, ModifierType::Bit, ModifierType::Bit, ModifierType::Bit
    };

    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
    auto numberParser = make_shared<ParsedNumber>();

    tokenizer->ToToken(numberString);

    auto counter = 0;
    for (auto token : tokenizer->Tokens())
    {
        try
        {
            numberParser->TryParse(token);
            EXPECT_EQ(modiferList[counter++], numberParser->Modifier());
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
       EXPECT_EQ(ModifierType::NoModifier, numberParser->Modifier());        
    }
}

TEST(TestNumberParser, CheckForInvalidNumericBasesCharacters)
{
    const auto stringContant = "H'hOhH, h'LLhoH, 0XHLooXX, 0x1112XHL, "
                               "D'AO9, d'12FF, 0D1234h, 0d12d.34, "
                               "O'9AFF, o'8Xo1, 0O0008, 0olabcc9, "
                               "B'12345678.abcde, b'0000000000012, 0B91881.9991, 0b67111AAdfeeeeee.e11110,  b'000000000001. "
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
        static_cast<string>("Invalid binary number 'b'000000000001.'"),
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
    const auto stringContant = "0xFFFF, H'1234, h'Aa, 0X00D1, 0o6654, O'233, o'1123, 0O31, 0d400, D'1023, d'674, 0D5610, 789, "
                               "0b11110000, B'0101010101010101, b'1111, 0B110011001100, 0b1011.0001, B'1100.1100.1100.1100,"
                               " b'11110000.1110, 0B11110000.11";
    auto tokenizer = make_shared<Tokenizer>();
    auto parsedNumber = make_shared<ParsedNumber>();
    tokenizer->ToToken(stringContant);

    parsedNumber->TryParse(tokenizer->Tokens()[0]); // 0xFFFF
    EXPECT_EQ(static_cast<uint16_t>(0xFFFF), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[1]); // H'1234
    EXPECT_EQ(static_cast<uint16_t>(0x1234), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[2]); // h'Aa
    EXPECT_EQ(static_cast<uint16_t>(0x00Aa), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[3]); // 0X00D1
    EXPECT_EQ(static_cast<uint16_t>(0x00D1), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[4]); // 0o6654
    EXPECT_EQ(static_cast<uint16_t>(3500), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[5]); // 0o233
    EXPECT_EQ(static_cast<uint16_t>(155), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[6]); // o'1123
    EXPECT_EQ(static_cast<uint16_t>(595), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[7]); // 0O31
    EXPECT_EQ(static_cast<uint16_t>(25), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[8]); // 0d400
    EXPECT_EQ(static_cast<uint16_t>(400), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[9]); // D'1023
    EXPECT_EQ(static_cast<uint16_t>(1023), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[10]); // d'674
    EXPECT_EQ(static_cast<uint16_t>(674), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[11]); // 0D5610
    EXPECT_EQ(static_cast<uint16_t>(5610), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[12]); // 789
    EXPECT_EQ(static_cast<uint16_t>(789), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[13]); // 0b11110000
    EXPECT_EQ(static_cast<uint16_t>(240), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[14]); // B'0101010101010101
    EXPECT_EQ(static_cast<uint16_t>(21845), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[15]); // b'1111
    EXPECT_EQ(static_cast<uint16_t>(0x0F), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[16]); // 0B110011001100
    EXPECT_EQ(static_cast<uint16_t>(3276), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[17]); // 0b1011.0001
    EXPECT_EQ(static_cast<uint16_t>(177), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[18]); // B'1100.1100.1100.1100
    EXPECT_EQ(static_cast<uint16_t>(52428), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[19]); // b'11110000.1110
    EXPECT_EQ(static_cast<uint16_t>(3854), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(tokenizer->Tokens()[20]); // 0B11110000.11
    EXPECT_EQ(static_cast<uint16_t>(963), static_cast<uint16_t>(parsedNumber->Value()));
}

TEST(TestNumberParser, StressTest)
{
    auto numericConstant = string("");
    auto tokenizer = make_shared<Tokenizer>();
    auto parsedNumber = make_shared<ParsedNumber>();

    for (uint32_t i = 0; i < numeric_limits<uint16_t>().max(); ++i)
    {
        std::stringstream ss;
        ss << "0x" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "H'" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
        
        ss.str("");
        ss << "0X" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
        
        ss.str("");
        ss << "h'" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "0o" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "O'" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
        
        ss.str("");
        ss << "0O" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
        
        ss.str("");
        ss << "o'" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "0d" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "D'" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
        
        ss.str("");
        ss << "0D" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
        
        ss.str("");
        ss << "d'" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
        
        ss.str("");
        ss << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
       
        ss.str("");
        ss << "0b" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "B'" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "0B" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "b'" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        parsedNumber->TryParse(tokenizer->Tokens()[0]);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));
    }
}
