#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Lexemes.h"
#include "DeclaredMember.h"
#include "LexemeToDeclaredMemberType.h"
#include "GBXAsmExceptions.h"

#include "TestUtils.h"

using namespace gbxasm::frontend;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::utilities;
using namespace gbxasm;
using namespace std;

using ::testing::Return;
using ::testing::_;

TEST(AssemblerTests_LexemeToMemberType, ConvertByte)
{
    auto type = LexemeToDeclaredMemberType::Convert(Lexemes::KeywordBYTE);
    ASSERT_EQ(TypeName::Byte, type);
}

TEST(AssemblerTests_LexemeToMemberType, ConvertWord)
{
    auto type = LexemeToDeclaredMemberType::Convert(Lexemes::KeywordWORD);
    ASSERT_EQ(TypeName::Word, type);
}

TEST(AssemblerTests_LexemeToMemberType, ConvertDoubleWord)
{
    auto type = LexemeToDeclaredMemberType::Convert(Lexemes::KeywordDWRD);
    ASSERT_EQ(TypeName::DoubleWord, type);
}

TEST(AssemblerTests_LexemeToMemberType, ConvertBool)
{
    auto type = LexemeToDeclaredMemberType::Convert(Lexemes::KeywordBOOL);
    ASSERT_EQ(TypeName::Bool, type);
}

TEST(AssemblerTests_LexemeToMemberType, ConvertChar)
{
    auto type = LexemeToDeclaredMemberType::Convert(Lexemes::KeywordCHAR);
    ASSERT_EQ(TypeName::Char, type);
}

TEST(AssemblerTests_LexemeToMemberType, ConvertString)
{
    auto type = LexemeToDeclaredMemberType::Convert(Lexemes::KeywordSTR);
    ASSERT_EQ(TypeName::String, type);
}

TEST(AssemblerTests_LexemeToMemberType, ConvertUnknownType)
{
    ASSERT_EXCEPTION( { [[maybe_unused]] auto type = LexemeToDeclaredMemberType::Convert("Unknown"); } , 
                    ConversionException, 
                    "Invalid type lexeme 'Unknown'");
}