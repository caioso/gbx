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
#include "../src/intermediate-representation/PackIntermediateRepresentation.h"
#include "../src/intermediate-representation/DeclaredMember.h"
#include "../src/GBXAsmExceptions.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::frontend::parsers;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::utilities;
using namespace std;

TEST(TestPackSyntacticAnalyzer, SanityCheckPackTokenization)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
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
                        // BGN
                        TokenType::KeywordBGN,
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
                        // BGN
                        Lexemes::KeywordBGN.c_str(),
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
                     //BGN
                     1llu, 
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
                     //BGN
                     2llu,
                     //    BYTE MY_BYTE_MEMBER
                     3llu, 3llu, 
                     //    WORD MY_WORD_MEMBER
                     4llu, 4llu, 
                     //    BOOL MY_BOOL_MEMBER
                     5llu, 5llu, 
                     //    CHAR MY_CHAR_MEMBER
                     6llu, 6llu, 
                     //    DWRD MY_DWRD_MEMBER
                     7llu, 7llu, 
                     //    STR MY_STRING_MEMBER[20]
                     8llu, 8llu, 8llu, 8llu, 8llu,
                     //END
                     9llu
                   };

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(TestPackSyntacticAnalyzer, ParsePack)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
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
}


TEST(TestPackSyntacticAnalyzer, ParsePackFormat2)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat3)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat4)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER[19990]\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat5)
{
    const string pack = "PACK \n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER[19990]\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat6)
{
    const string pack = "MY_PACK \n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER[19990]\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat7)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER[19990]\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat12)
{
    const string pack = "PACK MY_PACK \n"
                        "    BYTE MY_BYTE_MEMBER[19990]\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}


TEST(TestPackSyntacticAnalyzer, ParsePackFormat8)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    MY_BYTE MY_BYTE_MEMBER[19990]\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat9)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    BITE MY_BYTE_MEMBER\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat10)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    CHAR MY_BYTE_MEMBER[]\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, ParsePackFormat11)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER   STR MY_STRING_MEMBER\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(TestPackSyntacticAnalyzer, PackIntermediateRepresentationWithSingleMember)
{
    const string pack = "PACK FLAG_REGISTER\n"
                        "BGN\n"
                        "    BYTE FLAGS[2]\n"
                        "END";


    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PackSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    auto intermediateRepresentation = parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());

    auto packRepresentation = dynamic_pointer_cast<PackIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, packRepresentation);

    EXPECT_STREQ("FLAG_REGISTER", packRepresentation->Identifier().c_str());
    EXPECT_EQ(1llu, packRepresentation->Line());
    EXPECT_EQ(1llu, packRepresentation->Column());
    EXPECT_EQ(1llu, packRepresentation->Members().size());
    
    EXPECT_STREQ("FLAGS", packRepresentation->Members()[0].Identifier.c_str());
    EXPECT_EQ(TypeName::Byte, packRepresentation->Members()[0].Type.Name);
    EXPECT_TRUE(packRepresentation->Members()[0].IsArray);
    EXPECT_EQ("2", packRepresentation->Members()[0].ArrayLength);
}

TEST(TestPackSyntacticAnalyzer, PackIntermediateRepresentationWithMultipleMembers)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "    WORD MY_WORD_MEMBER\n"
                        "    BYTE MY_BYTE_ARRAY_MEMBER[0x100]\n"
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
    auto intermediateRepresentation = parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());

    auto packRepresentation = dynamic_pointer_cast<PackIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, packRepresentation);

    EXPECT_STREQ("MY_PACK", packRepresentation->Identifier().c_str());
    EXPECT_EQ(1llu, packRepresentation->Line());
    EXPECT_EQ(1llu, packRepresentation->Column());
    EXPECT_EQ(7llu, packRepresentation->Members().size());
    
    EXPECT_STREQ("MY_BYTE_MEMBER", packRepresentation->Members()[0].Identifier.c_str());
    EXPECT_EQ(TypeName::Byte, packRepresentation->Members()[0].Type.Name);
    EXPECT_FALSE(packRepresentation->Members()[0].IsArray);
    EXPECT_EQ("", packRepresentation->Members()[0].ArrayLength);
    
    EXPECT_STREQ("MY_WORD_MEMBER", packRepresentation->Members()[1].Identifier.c_str());
    EXPECT_EQ(TypeName::Word, packRepresentation->Members()[1].Type.Name);
    EXPECT_FALSE(packRepresentation->Members()[1].IsArray);
    EXPECT_EQ("", packRepresentation->Members()[1].ArrayLength);

    EXPECT_STREQ("MY_BYTE_ARRAY_MEMBER", packRepresentation->Members()[2].Identifier.c_str());
    EXPECT_EQ(TypeName::Byte, packRepresentation->Members()[2].Type.Name);
    EXPECT_TRUE(packRepresentation->Members()[2].IsArray);
    EXPECT_EQ("0x100", packRepresentation->Members()[2].ArrayLength);
    
    EXPECT_STREQ("MY_BOOL_MEMBER", packRepresentation->Members()[3].Identifier.c_str());
    EXPECT_EQ(TypeName::Bool, packRepresentation->Members()[3].Type.Name);
    EXPECT_FALSE(packRepresentation->Members()[3].IsArray);
    EXPECT_EQ("", packRepresentation->Members()[3].ArrayLength);
    
    EXPECT_STREQ("MY_CHAR_MEMBER", packRepresentation->Members()[4].Identifier.c_str());
    EXPECT_EQ(TypeName::Char, packRepresentation->Members()[4].Type.Name);
    EXPECT_FALSE(packRepresentation->Members()[4].IsArray);
    EXPECT_EQ("", packRepresentation->Members()[4].ArrayLength);
    
    EXPECT_STREQ("MY_DWRD_MEMBER", packRepresentation->Members()[5].Identifier.c_str());
    EXPECT_EQ(TypeName::DoubleWord, packRepresentation->Members()[5].Type.Name);
    EXPECT_FALSE(packRepresentation->Members()[5].IsArray);
    EXPECT_EQ("", packRepresentation->Members()[5].ArrayLength);
    
    EXPECT_STREQ("MY_STRING_MEMBER", packRepresentation->Members()[6].Identifier.c_str());
    EXPECT_EQ(TypeName::String, packRepresentation->Members()[6].Type.Name);
    EXPECT_TRUE(packRepresentation->Members()[6].IsArray);
    EXPECT_EQ("20", packRepresentation->Members()[6].ArrayLength);
}

