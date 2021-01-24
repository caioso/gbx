#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <string>

#include "../src/Lexer.h"
#include "../src/Lexemes.h"

using namespace gbxasm;
using namespace std;

TEST(TestLexer, Construction)
{
    auto lexer = make_shared<Lexer>();
}

TEST(TestLexer, AcquireReferenceOfTokensVector)
{
    auto lexer = make_shared<Lexer>();
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(0), tokens.size());
}

TEST(TestLexer, BasicTokenization)
{
    const string program = "";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(0), tokens.size());
}

TEST(TestLexer, BasicKeywordTokenization)
{
    const string program = "PACK";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateBasicLexeme)
{
    const string program = "PACK";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateLexemeWithOperator)
{
    const string program = "PACK+ +PACK +PACK+";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);

    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[1].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[2].Column);
    
    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(8), tokens[3].Column);

    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(13), tokens[4].Column);
    
    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[5].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[5].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[5].Line);
    EXPECT_EQ(static_cast<size_t>(14), tokens[5].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[6].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[6].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[6].Line);
    EXPECT_EQ(static_cast<size_t>(18), tokens[6].Column);
}

TEST(TestLexer, EvaluateLexemeWothMultiCharacterOperator)
{
    const string program = "PACK== PACK<=>";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);

    EXPECT_STREQ(Lexemes::OperatorEQUAL.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorEQUAL, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[1].Column);
}

TEST(TestLexer, EvaluateOperatorMixedWithKeyword)
{
    const string program = "P+ACK\nPAC=K P++K";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ("P", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(2), tokens[1].Column);

    EXPECT_STREQ("ACK", tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(3), tokens[2].Column);

    EXPECT_STREQ("PAC", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Column);
    
    EXPECT_STREQ(Lexemes::OperatorASSIGNMENT.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorASSIGNMENT, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(4), tokens[4].Column);

    EXPECT_STREQ("K", tokens[5].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[5].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[5].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[5].Column);

    EXPECT_STREQ("P", tokens[6].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[6].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[6].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[6].Column);
    
    EXPECT_STREQ("++", tokens[7].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[7].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[7].Line);
    EXPECT_EQ(static_cast<size_t>(8), tokens[7].Column);

    EXPECT_STREQ("K", tokens[8].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[8].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[8].Line);
    EXPECT_EQ(static_cast<size_t>(10), tokens[8].Column);
}

TEST(TestLexer, EvaluateSeparator)
{
    const string program = "PACK,(PACK)";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);

    EXPECT_STREQ(Lexemes::SeparatorCOMMA.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCOMMA, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[1].Column);

    EXPECT_STREQ(Lexemes::SeparatorOPENPARENTHESIS.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENPARENTHESIS, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(6), tokens[2].Column);

    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[3].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorCLOSEPARENTHESIS.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSEPARENTHESIS, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(11), tokens[4].Column);
}

TEST(TestLexer, TestNestedSeparatorsAndOperators)
{
    const string program = "{([PACK=PACK])}";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::SeparatorOPENCURLYBRACKETS.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENCURLYBRACKETS, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorOPENPARENTHESIS.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENPARENTHESIS, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(2), tokens[1].Column);

    EXPECT_STREQ(Lexemes::SeparatorOPENBRACKETS.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENBRACKETS, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(3), tokens[2].Column);

    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(4), tokens[3].Column);
    
    EXPECT_STREQ(Lexemes::OperatorASSIGNMENT.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorASSIGNMENT, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(8), tokens[4].Column);
    
    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[5].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[5].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[5].Line);
    EXPECT_EQ(static_cast<size_t>(9), tokens[5].Column);

    EXPECT_STREQ(Lexemes::SeparatorCLOSEBRACKETS.c_str(), tokens[6].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSEBRACKETS, tokens[6].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[6].Line);
    EXPECT_EQ(static_cast<size_t>(13), tokens[6].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorCLOSEPARENTHESIS.c_str(), tokens[7].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSEPARENTHESIS, tokens[7].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[7].Line);
    EXPECT_EQ(static_cast<size_t>(14), tokens[7].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorCLOSECURLYBRACKETS.c_str(), tokens[8].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSECURLYBRACKETS, tokens[8].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[8].Line);
    EXPECT_EQ(static_cast<size_t>(15), tokens[8].Column);
}


TEST(TestLexer, TestUnknownSeparator)
{
    const string program = "PACK? \\";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ("PACK?", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
   
    EXPECT_STREQ("\\", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::UnknownToken, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[1].Column);
}