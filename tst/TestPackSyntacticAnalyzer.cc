#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <utility>

#include "../src/frontend/LexicalAnalyzer.h"
#include "../src/frontend/Lexemes.h"
#include "../src/frontend/parsers/PackSyntacticAnalyzer.h"
#include "../src/interfaces/Construction.h"
#include "../src/language/ConstructionPack.h"
#include "../src/language/DeclaredMember.h"

using namespace gbxasm;
using namespace gbxasm::interfaces;
using namespace gbxasm::constructions;
using namespace gbxasm::parsers;
using namespace std;

TEST(TestPackSyntacticAnalyzer, SanityCheckPackTokenization)
{
    const string pack = "PACK MY_PACK\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "    WORD MY_WORD_MEMBER\n"
                        "    BOOL MY_BOOL_MEMBER\n"
                        "    CHAR MY_CHAR_MEMBER\n"
                        "    DWRD MY_DWRD_MEMBER\n"
                        "    STR MY_STRING_MEMBER[20]\n"
                        "END";


    auto lexer = make_shared<LexicalAnalyzer>();
    lexer->Tokenize(pack);
    auto tokens = lexer->Tokens();

    auto tokenTypes = { // PACK MY_PACK
                        TokenType::KeywordPACK, TokenType::Identifier,
                        // BYTE MY_BYTE_MEMBER
                        TokenType::KeywordBYTE, TokenType::Identifier,
                        // WORD MY_WORD_MEMBER
                        TokenType::KeywordWORD, TokenType::Identifier,
                        // BOOL MY_BOOL_MEMBER
                        TokenType::KeywordBOOL, TokenType::Identifier,
                        // CHAR MY_CHAR_MEMBER
                        TokenType::KeywordCHAR, TokenType::Identifier,
                        // DWRD MY_DWRD_MEMBER
                        TokenType::KeywordDWRD, TokenType::Identifier,
                        // STR MY_STRING_MEMBER[20]
                        TokenType::KeywordSTR, TokenType::Identifier, TokenType::SeparatorOPENBRACKETS, TokenType::LiteralNumericDECIMAL, TokenType::SeparatorCLOSEBRACKETS,
                        // END
                        TokenType::KeywordEND
                      };

    auto lexemes = { // PACK MY_PACK
                        Lexemes::KeywordPACK.c_str(), "MY_PACK",
                        // BYTE MY_BYTE_MEMBER
                        Lexemes::KeywordBYTE.c_str(), "MY_BYTE_MEMBER",
                        // WORD MY_WORD_MEMBER
                        Lexemes::KeywordWORD.c_str(), "MY_WORD_MEMBER",
                        // BOOL MY_BOOL_MEMBER
                        Lexemes::KeywordBOOL.c_str(), "MY_BOOL_MEMBER",
                        // CHAR MY_CHAR_MEMBER
                        Lexemes::KeywordCHAR.c_str(), "MY_CHAR_MEMBER",
                        // DWRD MY_DWRD_MEMBER
                        Lexemes::KeywordDWRD.c_str(), "MY_DWRD_MEMBER",
                        // STR MY_STRING_MEMBER[20]
                        Lexemes::KeywordSTR.c_str(), "MY_STRING_MEMBER", Lexemes::SeparatorOPENBRACKETS.c_str(), "20", Lexemes::SeparatorCLOSEBRACKETS.c_str(),
                        // END
                        Lexemes::KeywordEND.c_str()
                      };

    auto columns = { //PACK MY_PACK
                     1llu, 6llu,
                     //    BYTE MY_BYTE_MEMBER
                     5llu, 10llu, 
                     //    WORD MY_WORD_MEMBER
                     5llu, 10llu, 
                     //    BOOL MY_BOOL_MEMBER
                     5llu, 10llu, 
                     //    CHAR MY_CHAR_MEMBER
                     5llu, 10llu, 
                     //    DWRD MY_DWRD_MEMBER
                     5llu, 10llu, 
                     //    STR MY_STRING_MEMBER[20]
                     5llu, 9llu, 25llu, 26llu, 28llu,
                     //END
                     1llu,
                   };
    
    auto lines = {   //PACK MY_PACK
                     1llu, 1llu,
                     //    BYTE MY_BYTE_MEMBER
                     2llu, 2llu, 
                     //    WORD MY_WORD_MEMBER
                     3llu, 3llu, 
                     //    BOOL MY_BOOL_MEMBER
                     4llu, 4llu, 
                     //    CHAR MY_CHAR_MEMBER
                     5llu, 5llu, 
                     //    DWRD MY_DWRD_MEMBER
                     6llu, 6llu, 
                     //    STR MY_STRING_MEMBER[20]
                     7llu, 7llu, 7llu, 7llu, 7llu,
                     //END
                     8llu
                   };

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(TestPackSyntacticAnalyzer, ParsePackSimple)
{
    const string pack = "PACK MY_PACK\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "    WORD MY_WORD_MEMBER\n"
                        "    BOOL MY_BOOL_MEMBER\n"
                        "    CHAR MY_CHAR_MEMBER\n"
                        "    DWRD MY_DWRD_MEMBER\n"
                        "    STR MY_STRING_MEMBER[20]\n"
                        "END";


    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());

    /*EXPECT_EQ(1llu, acceptedConstructions.size());
    EXPECT_EQ(ConstructionType::Pack, acceptedConstructions[0].Type);
    EXPECT_EQ(1llu, acceptedConstructions[0].Construction->Line());
    EXPECT_EQ(1llu, acceptedConstructions[0].Construction->Column());
    
    // Construcion-specific checks
    auto packConstruction = static_pointer_cast<ConstructionPack>(acceptedConstructions[0].Construction);
    EXPECT_STREQ("MY_PACK", packConstruction->Identifier().c_str());
    EXPECT_EQ(6llu, packConstruction->Members().size());

    // Check each member
    EXPECT_EQ(TypeName::Byte, packConstruction->Members()[0].Type.Name);
    EXPECT_EQ(1llu, packConstruction->Members()[0].Type.Size);
    EXPECT_FALSE(packConstruction->Members()[0].IsArray);
    EXPECT_STREQ("MY_BYTE_MEMBER", packConstruction->Members()[0].Identifier.c_str());

    EXPECT_EQ(TypeName::Word, packConstruction->Members()[1].Type.Name);
    EXPECT_EQ(2llu, packConstruction->Members()[1].Type.Size);
    EXPECT_FALSE(packConstruction->Members()[1].IsArray);
    EXPECT_STREQ("MY_WORD_MEMBER", packConstruction->Members()[1].Identifier.c_str());

    EXPECT_EQ(TypeName::Bool, packConstruction->Members()[2].Type.Name);
    EXPECT_EQ(1llu, packConstruction->Members()[2].Type.Size);
    EXPECT_FALSE(packConstruction->Members()[2].IsArray);
    EXPECT_STREQ("MY_BOOL_MEMBER", packConstruction->Members()[2].Identifier.c_str());

    EXPECT_EQ(TypeName::Char, packConstruction->Members()[3].Type.Name);
    EXPECT_EQ(1llu, packConstruction->Members()[3].Type.Size);
    EXPECT_FALSE(packConstruction->Members()[3].IsArray);
    EXPECT_STREQ("MY_CHAR_MEMBER", packConstruction->Members()[3].Identifier.c_str());

    EXPECT_EQ(TypeName::DoubleWord, packConstruction->Members()[4].Type.Name);
    EXPECT_EQ(4llu, packConstruction->Members()[4].Type.Size);
    EXPECT_FALSE(packConstruction->Members()[4].IsArray);
    EXPECT_STREQ("MY_DWRD_MEMBER", packConstruction->Members()[4].Identifier.c_str());

    EXPECT_EQ(TypeName::String, packConstruction->Members()[5].Type.Name);
    EXPECT_EQ(1llu, packConstruction->Members()[5].Type.Size);
    EXPECT_EQ(20llu, packConstruction->Members()[5].ArrayLength);
    EXPECT_TRUE(packConstruction->Members()[5].IsArray);
    EXPECT_STREQ("MY_STRING_MEMBER", packConstruction->Members()[5].Identifier.c_str());*/
}