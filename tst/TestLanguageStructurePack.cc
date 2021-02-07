#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "../src/Lexer.h"
#include "../src/Lexemes.h"

using namespace gbxasm;
using namespace std;

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

TEST(TestLanguageStructurePack, SimplePack)
{
    const string pack = "PACK MY_PACK\n"
                        "    BYTE MY_BYTE_MEMBER\n"
                        "    WORD MY_WORD_MEMBER\n"
                        "    BOOL MY_BOOL_MEMBER\n"
                        "    CHAR MY_CHAR_MEMBER\n"
                        "    DWRD MY_DWRD_MEMBER\n"
                        "    STR MY_STRING_MEMBER[20]\n"
                        "END";


    auto lexer = make_shared<Lexer>();
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