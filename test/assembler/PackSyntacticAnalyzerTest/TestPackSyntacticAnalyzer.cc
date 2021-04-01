#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <utility>

#include "Construction.h"
#include "DeclaredMember.h"
#include "GBXAsmExceptions.h"
#include "LexicalAnalyzer.h"
#include "Lexemes.h"
#include "PackSyntacticAnalyzer.h"
#include "PackIntermediateRepresentation.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::frontend::parsers;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::utilities;
using namespace std;

TEST(AssemblerTests_PACKSyntacticAnalyzer, SanityCheckPackTokenization)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "    WORD MY_WORD_MEMBER\n"
                        "    BOOL MY_BOOL_MEMBER\n"
                        "    CHAR MY_CHAR_MEMBER\n"
                        "    DWRD MY_DWRD_MEMBER\n"
                        "    STR[20] MY_STRING_MEMBER\n"
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
                        // STR[20] MY_STRING_MEMBER
                        TokenType::KeywordSTR, TokenType::SeparatorOPENBRACKETS, TokenType::LiteralNumericDECIMAL, TokenType::SeparatorCLOSEBRACKETS, TokenType::Identifier, 
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
                        // STR[20] MY_STRING_MEMBER
                        Lexemes::KeywordSTR.c_str(), Lexemes::SeparatorOPENBRACKETS.c_str(), "20", Lexemes::SeparatorCLOSEBRACKETS.c_str(), "MY_STRING_MEMBER",
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
                     //    STR[20] MY_STRING_MEMBER
                     5llu, 8llu, 9llu, 11llu, 13llu,
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
                     //    STR[20] MY_STRING_MEMBER
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

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePack)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "    WORD MY_WORD_MEMBER\n"
                        "    BOOL MY_BOOL_MEMBER\n"
                        "    CHAR MY_CHAR_MEMBER\n"
                        "    DWRD MY_DWRD_MEMBER\n"
                        "    STR[20] MY_STRING_MEMBER\n"
                        "END";


    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}


TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat2)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat3)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat4)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE[19990] MY_BYTE_MEMBER\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat5)
{
    const string pack = "PACK \n"
                        "BGN\n"
                        "    BYTE[19990] MY_BYTE_MEMBER\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat6)
{
    const string pack = "MY_PACK \n"
                        "BGN\n"
                        "    BYTE[19990] MY_BYTE_MEMBER\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat7)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    BYTE[19990] MY_BYTE_MEMBER\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat12)
{
    const string pack = "PACK MY_PACK \n"
                        "    BYTE[19990] MY_BYTE_MEMBER\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}


TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat8)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    MY_BYTE[19990] MY_BYTE_MEMBER\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat9)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    BITE MY_BYTE_MEMBER\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat10)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    CHAR MY_BYTE_MEMBER[]\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat14)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    CHAR MY_BYTE_MEMBER[100]\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat13)
{
    const string pack = "PACK MY_PACK \n"
                        "BGN\n"
                        "    CHAR[] MY_BYTE_MEMBER\n"
                        "END\n";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_FALSE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, ParsePackFormat11)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER   STR MY_STRING_MEMBER\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, PACKIntermediateRepresentationWithSingleMember)
{
    const string pack = "PACK FLAG_REGISTER\n"
                        "BGN\n"
                        "    BYTE[2] FLAGS\n"
                        "END";


    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    auto intermediateRepresentation = parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());

    auto packRepresentation = dynamic_pointer_cast<PACKIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, packRepresentation);

    EXPECT_STREQ("FLAG_REGISTER", string(packRepresentation->Identifier()).c_str());
    EXPECT_EQ(1llu, packRepresentation->Line());
    EXPECT_EQ(1llu, packRepresentation->Column());
    EXPECT_EQ(1llu, packRepresentation->Members().size());
    
    EXPECT_STREQ("FLAGS", packRepresentation->Members()[0].Identifier.c_str());
    EXPECT_EQ(TypeName::Byte, packRepresentation->Members()[0].Type.Name);
    EXPECT_TRUE(packRepresentation->Members()[0].IsArray);
    EXPECT_EQ("2", packRepresentation->Members()[0].ArrayLength);
}

TEST(AssemblerTests_PACKSyntacticAnalyzer, PACKIntermediateRepresentationWithMultipleMembers)
{
    const string pack = "PACK MY_PACK\n"
                        "BGN\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "    WORD MY_WORD_MEMBER\n"
                        "    BYTE[0x100] MY_BYTE_ARRAY_MEMBER\n"
                        "    BOOL MY_BOOL_MEMBER\n"
                        "    CHAR MY_CHAR_MEMBER\n"
                        "    DWRD MY_DWRD_MEMBER\n"
                        "    STR[20] MY_STRING_MEMBER\n"
                        "END";


    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<PACKSyntacticAnalyzer>();
    lexer->Tokenize(pack);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    auto intermediateRepresentation = parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());

    auto packRepresentation = dynamic_pointer_cast<PACKIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, packRepresentation);

    EXPECT_STREQ("MY_PACK", string(packRepresentation->Identifier()).c_str());
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

