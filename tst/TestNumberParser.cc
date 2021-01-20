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
         tokenizer->ToToken(numberString);

    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
        numberParser->TryParse(*iterator);
        EXPECT_EQ(static_cast<uint32_t>(1234), numberParser->Value());
        EXPECT_EQ(NumericBase::Decimal, numberParser->Base());
    }
}

TEST(TestNumberParser, ParseInvalidDecimalNumbers)
{
    auto numberString = static_cast<string>("    &1234   ");
    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
        ASSERT_EXCEPTION( { numberParser->TryParse(*iterator); }, 
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
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
        try
        {
            numberParser->TryParse(*iterator);
        }
        catch (const ParsedNumberException& e)
        {
            cout << "Exception Message: " << e.what() << '\n';
            FAIL();
        }
        
    }
}

TEST(TestNumberParser, ParserEvaluateNumericType)
{
    const auto stringContant = "H'FFFF, h'0011, 0xFF, 0xF12, 0X1FF, H'000000FFF, "
                               "O'377, 0o400, O'77, 0O551, 0o0001,  0o00600, "
                               "255, 1024, 0d000256, D'0000140, 00001,  003000, "
                               "B'1111.0000.1111, b'0000000000.11111111, 0b0000.0001.0000.0000, 0B0000100, "
                               "H'FFFFF, D'98523, O'1234561234, B'1111.1111.1111.1111.1111.0000.0000";
    
    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    NumericType numericTypeList[] = 
    {
        NumericType::Type16Bit, NumericType::Type8Bit, NumericType::Type8Bit, NumericType::Type16Bit, NumericType::Type16Bit, NumericType::Type16Bit, 
        NumericType::Type8Bit,  NumericType::Type16Bit, NumericType::Type8Bit, NumericType::Type16Bit, NumericType::Type8Bit, NumericType::Type16Bit,
        NumericType::Type8Bit, NumericType::Type16Bit, NumericType::Type16Bit, NumericType::Type8Bit, NumericType::Type8Bit, NumericType::Type16Bit,
        NumericType::Type16Bit, NumericType::Type8Bit, NumericType::Type16Bit, NumericType::Type8Bit,
        NumericType::Type32Bit, NumericType::Type32Bit, NumericType::Type32Bit, NumericType::Type32Bit,
    };

    auto counter = 0;
    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
        try
        {
            numberParser->TryParse(*iterator);
            EXPECT_EQ(numericTypeList[counter++], numberParser->Type());
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
    const auto stringContant = "H'ABCD, H'F00:H, H'FF00:L, H'AADD:h, H'DFF:l, "
                               "H'0110:high, H'1AAD:HIGH, H'1AAD:LOW, H'2033:low, H'E8:H, H'67:L, H'AABBCCDD:H, H'AABBCCDD:LOW";

    ModifierType modiferList[] = 
    {
        ModifierType::NoModifier, ModifierType::HigherHalf, ModifierType::LowerHalf, ModifierType::HigherHalf, ModifierType::LowerHalf,
        ModifierType::HigherHalf, ModifierType::HigherHalf, ModifierType::LowerHalf, ModifierType::LowerHalf,
        ModifierType::HigherHalf, ModifierType::LowerHalf, 
        ModifierType::HigherHalf, ModifierType::LowerHalf
    };

    uint32_t originalValue[] = 
    {
        0xABCD, 0x0F00, 0xFF00, 0xAADD, 0x0DFF, 0x0110, 0x1AAD, 0x1AAD, 0x2033,  0xE8, 0x67,
        0xAABBCCDD, 0xAABBCCDD
    };

    uint16_t modifiedValue[] = 
    {
        0xABCD, 0x0F, 0x00, 0xAA, 0x0FF, 0x01, 0x1A, 0xAD, 0x33, 0x0E, 0x07, 0xAABB, 0xCCDD
    };

    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    auto counter = 0;
    
    for (; iterator != end(tokenizer->Tokens()); ++iterator)    
    {
        try
        {
            numberParser->TryParse(*iterator);
            EXPECT_EQ(modiferList[counter], numberParser->Modifier());
            EXPECT_EQ(originalValue[counter], numberParser->Value());
            EXPECT_EQ(modifiedValue[counter++], numberParser->ModifiedValue());
        }
        catch (const ParsedNumberException& e)
        {
            cout << "Exception Message: " << e.what() << '\n';
            FAIL();
        }
        
    }
}

TEST(TestNumberParser, EvaluateBitSuffix)
{
    const auto stringContant = "H'AAAA:BIT[0], H'AAAA:BIT[1], H'AAAA:BIT[2], H'AAAA:BIT[3], "
                               "H'AAAA:BIT[4], H'AAAA:BIT[5], H'AAAA:BIT[6], H'AAAA:BIT[7], "
                               "H'AAAA:BIT[8], H'AAAA:BIT[9], H'AAAA:BIT[H'A], H'AAAA:BIT[O'13], "
                               "H'AAAA:BIT[B'1100], H'AAAA:BIT[D'13], H'AAAA:BIT[0xE], H'AAAA:BIT[15], ";

    uint16_t modifiedValue[] = 
    {
        0x00, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01,
        0x00, 0x01, 0x00, 0x01
    };

    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    auto counter = 0;
    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
        try
        {
            numberParser->TryParse(*iterator);
            EXPECT_EQ(ModifierType::Bit, numberParser->Modifier());
            EXPECT_EQ(static_cast<uint32_t>(0xAAAA), numberParser->Value());
            EXPECT_EQ(modifiedValue[counter++], numberParser->ModifiedValue());
        }
        catch (const ParsedNumberException& e)
        {
            cout << "Exception Message: " << e.what() << '\n';
            FAIL();
        }
        
    }
}

TEST(TestNumberParser, EvaluateComposedBitSuffix)
{
    const auto stringContant = "H'F411:BIT[H'0F:LOW], H'00100000:BIT[H'1400:HIGH], "
                               "H'0002:BIT[H'0E:BIT[1]], H'FEFE:BIT[H'0100:BIT[H'0009FFFF:HIGH]] ";

    uint32_t originalValue[] = 
    {
        0xF411, 0x00100000, 0x0002, 0xFEFE
    };

    uint16_t modifiedValue[] = 
    {
        0x01, 0x01, 0x01, 0x00
    };

    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    auto counter = 0;
    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
        try
        {
            numberParser->TryParse(*iterator);
            cout << (*iterator).TokenWithoutDelimiter << '\n';
            EXPECT_EQ(ModifierType::Bit, numberParser->Modifier());
            EXPECT_EQ(originalValue[counter], numberParser->Value());
            EXPECT_EQ(modifiedValue[counter++], numberParser->ModifiedValue());
        }
        catch (const ParsedNumberException& e)
        {
            cout << "Exception Message: " << e.what() << '\n';
            FAIL();
        }
        
    }
}

TEST(TestNumberParser, EvaluateComposedBitSuffixWithSpaces)
{
    const auto stringContant = "H'8881:BIT [ 0 ] ";

    uint32_t originalValue[] = 
    {
        0x8881
    };

    uint16_t modifiedValue[] = 
    {
        0x01
    };

    auto numberString = static_cast<string>(stringContant);
    auto tokenizer = make_shared<Tokenizer>();
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    auto counter = 0;
    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
        try
        {
            numberParser->TryParse(*iterator);
            cout << (*iterator).TokenWithoutDelimiter << '\n';
            EXPECT_EQ(ModifierType::Bit, numberParser->Modifier());
            EXPECT_EQ(originalValue[counter], numberParser->Value());
            EXPECT_EQ(modifiedValue[counter++], numberParser->ModifiedValue());
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
         tokenizer->ToToken(numberString);
    
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    auto counter = 0;
    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
       numberParser->TryParse(*iterator);

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
         tokenizer->ToToken(numberString);
    
    auto counter = 0;
    auto iterator = begin(tokenizer->Tokens());
    auto numberParser = make_shared<ParsedNumber>(iterator);

    for (; iterator != end(tokenizer->Tokens()); ++iterator)
    {
       ASSERT_EXCEPTION( { numberParser->TryParse((*iterator)); }, 
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
         tokenizer->ToToken(stringContant);
    
    auto iterator = begin(tokenizer->Tokens());
    auto parsedNumber = make_shared<ParsedNumber>(iterator);

    parsedNumber->TryParse(*iterator); // 0xFFFF
    EXPECT_EQ(static_cast<uint16_t>(0xFFFF), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 1)); // H'1234
    EXPECT_EQ(static_cast<uint16_t>(0x1234), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 2)); // h'Aa
    EXPECT_EQ(static_cast<uint16_t>(0x00Aa), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 3)); // 0X00D1
    EXPECT_EQ(static_cast<uint16_t>(0x00D1), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 4)); // 0o6654
    EXPECT_EQ(static_cast<uint16_t>(3500), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 5)); // 0o233
    EXPECT_EQ(static_cast<uint16_t>(155), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 6)); // o'1123
    EXPECT_EQ(static_cast<uint16_t>(595), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 7)); // 0O31
    EXPECT_EQ(static_cast<uint16_t>(25), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 8)); // 0d400
    EXPECT_EQ(static_cast<uint16_t>(400), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 9)); // D'1023
    EXPECT_EQ(static_cast<uint16_t>(1023), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 10)); // d'674
    EXPECT_EQ(static_cast<uint16_t>(674), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 11)); // 0D5610
    EXPECT_EQ(static_cast<uint16_t>(5610), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 12)); // 789
    EXPECT_EQ(static_cast<uint16_t>(789), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 13)); // 0b11110000
    EXPECT_EQ(static_cast<uint16_t>(240), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 14)); // B'0101010101010101
    EXPECT_EQ(static_cast<uint16_t>(21845), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 15)); // b'1111
    EXPECT_EQ(static_cast<uint16_t>(0x0F), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 16)); // 0B110011001100
    EXPECT_EQ(static_cast<uint16_t>(3276), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 17)); // 0b1011.0001
    EXPECT_EQ(static_cast<uint16_t>(177), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 18)); // B'1100.1100.1100.1100
    EXPECT_EQ(static_cast<uint16_t>(52428), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 19)); // b'11110000.1110
    EXPECT_EQ(static_cast<uint16_t>(3854), static_cast<uint16_t>(parsedNumber->Value()));
    parsedNumber->TryParse(*(iterator + 20)); // 0B11110000.11
    EXPECT_EQ(static_cast<uint16_t>(963), static_cast<uint16_t>(parsedNumber->Value()));
}

TEST(TestNumberParser, StressTest)
{
    return;
    auto numericConstant = string("");
    auto tokenizer = make_shared<Tokenizer>();

    for (uint32_t i = 0; i < numeric_limits<uint16_t>().max(); ++i)
    {
        std::stringstream ss;
        ss << "0x" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator = begin(tokenizer->Tokens());
        auto parsedNumber = make_shared<ParsedNumber>(iterator);
        parsedNumber->TryParse(*iterator);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber->Value()));

        ss.str("");
        ss << "H'" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator2 = begin(tokenizer->Tokens());
        auto parsedNumber2 = make_shared<ParsedNumber>(iterator2);
        parsedNumber2->TryParse(*iterator2);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber2->Value()));
        
        ss.str("");
        ss << "0X" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator3 = begin(tokenizer->Tokens());
        auto parsedNumber3 = make_shared<ParsedNumber>(iterator3);
        parsedNumber3->TryParse(*iterator3);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber3->Value()));
        
        ss.str("");
        ss << "h'" << hex << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator4 = begin(tokenizer->Tokens());
        auto parsedNumber4 = make_shared<ParsedNumber>(iterator4);
        parsedNumber4->TryParse(*iterator4);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber4->Value()));

        ss.str("");
        ss << "0o" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator5 = begin(tokenizer->Tokens());
        auto parsedNumber5 = make_shared<ParsedNumber>(iterator5);
        parsedNumber5->TryParse(*iterator5);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber5->Value()));

        ss.str("");
        ss << "O'" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator17 = begin(tokenizer->Tokens());
        auto parsedNumber17 = make_shared<ParsedNumber>(iterator17);
        parsedNumber17->TryParse(*iterator17);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber17->Value()));
        
        ss.str("");
        ss << "0O" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator6 = begin(tokenizer->Tokens());
        auto parsedNumber6 = make_shared<ParsedNumber>(iterator6);
        parsedNumber6->TryParse(*iterator6);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber6->Value()));
        
        ss.str("");
        ss << "o'" << oct << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator7 = begin(tokenizer->Tokens());
        auto parsedNumber7 = make_shared<ParsedNumber>(iterator7);
        parsedNumber7->TryParse(*iterator7);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber7->Value()));

        ss.str("");
        ss << "0d" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator8 = begin(tokenizer->Tokens());
        auto parsedNumber8 = make_shared<ParsedNumber>(iterator8);
        parsedNumber8->TryParse(*iterator8);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber8->Value()));

        ss.str("");
        ss << "D'" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator9 = begin(tokenizer->Tokens());
        auto parsedNumber9 = make_shared<ParsedNumber>(iterator9);
        parsedNumber9->TryParse(*iterator9);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber9->Value()));
        
        ss.str("");
        ss << "0D" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator10 = begin(tokenizer->Tokens());
        auto parsedNumber10 = make_shared<ParsedNumber>(iterator10);
        parsedNumber10->TryParse(*iterator10);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber10->Value()));
        
        ss.str("");
        ss << "d'" << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator11 = begin(tokenizer->Tokens());
        auto parsedNumber11 = make_shared<ParsedNumber>(iterator11);
        parsedNumber11->TryParse(*iterator11);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber11->Value()));
        
        ss.str("");
        ss << dec << i; 
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator12 = begin(tokenizer->Tokens());
        auto parsedNumber12 = make_shared<ParsedNumber>(iterator12);
        parsedNumber12->TryParse(*iterator12);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber12->Value()));
       
        ss.str("");
        ss << "0b" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator13 = begin(tokenizer->Tokens());
        auto parsedNumber13 = make_shared<ParsedNumber>(iterator13);
        parsedNumber13->TryParse(*iterator13);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber13->Value()));

        ss.str("");
        ss << "B'" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator14 = begin(tokenizer->Tokens());
        auto parsedNumber14 = make_shared<ParsedNumber>(iterator14);
        parsedNumber14->TryParse(*iterator14);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber14->Value()));

        ss.str("");
        ss << "0B" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator15 = begin(tokenizer->Tokens());
        auto parsedNumber15 = make_shared<ParsedNumber>(iterator15);
        parsedNumber15->TryParse(*iterator15);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber15->Value()));

        ss.str("");
        ss << "b'" << ToBinary(i);
        numericConstant = string(ss.str());
        tokenizer->ToToken(numericConstant);
        auto iterator16 = begin(tokenizer->Tokens());
        auto parsedNumber16 = make_shared<ParsedNumber>(iterator16);
        parsedNumber16->TryParse(*iterator16);
        EXPECT_EQ(static_cast<uint32_t>(i), static_cast<uint32_t>(parsedNumber16->Value()));
    }
}
