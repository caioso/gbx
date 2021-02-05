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
    EXPECT_EQ(TokenType::Identifier, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(2), tokens[1].Column);

    EXPECT_STREQ("ACK", tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(3), tokens[2].Column);

    EXPECT_STREQ("PAC", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Column);
    
    EXPECT_STREQ(Lexemes::OperatorASSIGNMENT.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorASSIGNMENT, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(4), tokens[4].Column);

    EXPECT_STREQ("K", tokens[5].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[5].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[5].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[5].Column);

    EXPECT_STREQ("P", tokens[6].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[6].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[6].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[6].Column);
    
    EXPECT_STREQ("++", tokens[7].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[7].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[7].Line);
    EXPECT_EQ(static_cast<size_t>(8), tokens[7].Column);

    EXPECT_STREQ("K", tokens[8].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[8].Type);
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
    EXPECT_EQ(TokenType::Identifier, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
   
    EXPECT_STREQ("\\", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[1].Column);
}

TEST(TestLexer, EvaluateAllKeywords)
{
    const string program = "PACK\n"
                           "FUNC\n"
                           "END\n"
                           "DECL\n"
                           "BOOL\n"
                           "CHAR\n"
                           "BYTE\n"
                           "WORD\n"
                           "DWORD\n"
                           "STR\n"
                           "AS\n"
                           "CONST\n"
                           "FREE\n"
                           "IF\n"
                           "THEN\n"
                           "ELSE\n"
                           "WITH\n"
                           "REPT\n"
                           "TIMES\n"
                           "NEXT\n"
                           "EXIT\n"
                           "WHEN\n"
                           "IS\n"
                           "WHILE\n"
                           "ALIAS\n"
                           "TRY\n"
                           "CATCH\n"
                           "ABORT\n"
                           "TEST\n"
                           "MACRO\n"
                           "MOVE\n"
                           "HIGH\n"
                           "LOW\n"
                           "CHECK\n"
                           "ASSRT\n"
                           "PASS\n"
                           "FAIL\n";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    auto keywordsString = {Lexemes::KeywordPACK, Lexemes::KeywordFUNC, Lexemes::KeywordEND, Lexemes::KeywordDECL,
                           Lexemes::KeywordBOOL, Lexemes::KeywordCHAR, Lexemes::KeywordBYTE, Lexemes::KeywordWORD,
                           Lexemes::KeywordDWORD, Lexemes::KeywordSTR, Lexemes::KeywordAS, Lexemes::KeywordCONST,
                           Lexemes::KeywordFREE, Lexemes::KeywordIF, Lexemes::KeywordTHEN, Lexemes::KeywordELSE,
                           Lexemes::KeywordWITH, Lexemes::KeywordREPT, Lexemes::KeywordTIMES, Lexemes::KeywordNEXT,
                           Lexemes::KeywordEXIT, Lexemes::KeywordWHEN, Lexemes::KeywordIS, Lexemes::KeywordWHILE, 
                           Lexemes::KeywordALIAS, Lexemes::KeywordTRY, Lexemes::KeywordCATCH, Lexemes::KeywordABORT, 
                           Lexemes::KeywordTEST, Lexemes::KeywordMACRO, Lexemes::KeywordMOVE, Lexemes::KeywordHIGH,
                           Lexemes::KeywordLOW, Lexemes::KeywordCHECK, Lexemes::KeywordASSRT, Lexemes::KeywordPASS, 
                           Lexemes::KeywordFAIL};

    auto keywordsTokens = {TokenType::KeywordPACK, TokenType::KeywordFUNC, TokenType::KeywordEND, TokenType::KeywordDECL,
                           TokenType::KeywordBOOL, TokenType::KeywordCHAR, TokenType::KeywordBYTE, TokenType::KeywordWORD, 
                           TokenType::KeywordDWORD, TokenType::KeywordSTR, TokenType::KeywordAS, TokenType::KeywordCONST,
                           TokenType::KeywordFREE, TokenType::KeywordIF, TokenType::KeywordTHEN, TokenType::KeywordELSE,
                           TokenType::KeywordWITH, TokenType::KeywordREPT, TokenType::KeywordTIMES, TokenType::KeywordNEXT,
                           TokenType::KeywordEXIT, TokenType::KeywordWHEN, TokenType::KeywordIS, TokenType::KeywordWHILE,
                           TokenType::KeywordALIAS, TokenType::KeywordTRY, TokenType::KeywordCATCH, TokenType::KeywordABORT,
                           TokenType::KeywordTEST, TokenType::KeywordMACRO, TokenType::KeywordMOVE, TokenType::KeywordHIGH,
                           TokenType::KeywordLOW, TokenType::KeywordCHECK, TokenType::KeywordASSRT, TokenType::KeywordPASS, 
                           TokenType::KeywordFAIL};

    auto counter = 0;
    for (auto i = static_cast<size_t>(0); i < keywordsString.size(); ++i)
    {
        EXPECT_STREQ((*(begin(keywordsString) + i)).c_str(), tokens[counter].Lexeme.c_str());
        EXPECT_EQ(*(begin(keywordsTokens) + i), tokens[counter].Type);
        EXPECT_EQ(static_cast<size_t>(counter + 1), tokens[counter].Line);
        EXPECT_EQ(static_cast<size_t>(1), tokens[counter++].Column);
    }
}

TEST(TestLexer, EvaluateAllOperators)
{
    const string program = "=\n"
                           "==\n"
                           "+\n"
                           "<=>\n"
                           "-\n"
                           "*\n"
                           "&\n"
                           "|\n"
                           "~\n"
                           "^\n"
                           "<<\n"
                           ">>\n"
                           "!=\n"
                           "&&\n"
                           "||\n"
                           "<\n"
                           ">\n"
                           "<=\n"
                           ">=\n"
                           "!\n"
                           "@\n"
                           ":\n"
                           ".\n";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    auto operatorString = {Lexemes::OperatorASSIGNMENT, Lexemes::OperatorEQUAL, Lexemes::OperatorPLUS, Lexemes::OperatorTRHEEWAYCOMPARISON,
                           Lexemes::OperatorMINUS, Lexemes::OperatorMULTIPLICATION, Lexemes::OperatorBITWISEAND, Lexemes::OperatorBITWISEOR,
                           Lexemes::OperatorBITWISENOT, Lexemes::OperatorBITWISEXOR, Lexemes::OperatorLEFTSHIFT, Lexemes::OperatorRIGHTSHIFT,
                           Lexemes::OperatorDIFFERENT, Lexemes::OperatorLOGICAND, Lexemes::OperatorLOGICOR, Lexemes::OperatorLESSTHAN,
                           Lexemes::OperatorGREATERTHAN, Lexemes::OperatorLESSTHANOREQUALTO, Lexemes::OperatorGREATERTHANOREQUALTO,
                           Lexemes::OperatorLOGICNOT, Lexemes::OperatorAT, Lexemes::OperatorSEMICOLON, Lexemes::OperatorDOT};
    
    auto operatorTokens = {TokenType::OperatorASSIGNMENT, TokenType::OperatorEQUAL, TokenType::OperatorPLUS, TokenType::OperatorTHREEWAYCOMPARISON,
                           TokenType::OperatorMINUS, TokenType::OperatorMULTIPLICATION, TokenType::OperatorBITWISEAND, TokenType::OperatorBITWISEOR,
                           TokenType::OperatorBITWISENOT, TokenType::OperatorBITWISEXOR, TokenType::OperatorLEFTSHIFT, TokenType::OperatorRIGHTSHIFT, 
                           TokenType::OperatorDIFFERENT, TokenType::OperatorLOGICAND, TokenType::OperatorLOGICOR, TokenType::OperatorLESSTHAN,
                           TokenType::OperatorGREATERTHAN, TokenType::OperatorLESSTHANOREQUALTO, TokenType::OperatorGREATERTHANOREQUALTO,
                           TokenType::OperatorLOGICNOT, TokenType::OperatorAT, TokenType::OperatorSEMICOLON, TokenType::OperatorDOT};

    auto counter = 0;
    for (auto i = static_cast<size_t>(0); i < operatorString.size(); ++i)
    {
        EXPECT_STREQ((*(begin(operatorString) + i)).c_str(), tokens[counter].Lexeme.c_str());
        EXPECT_EQ(*(begin(operatorTokens) + i), tokens[counter].Type);
        EXPECT_EQ(static_cast<size_t>(counter + 1), tokens[counter].Line);
        EXPECT_EQ(static_cast<size_t>(1), tokens[counter++].Column);
    }
}

TEST(TestLexer, EvaluateNumericLiterals)
{
    const string program = "0x12AB\n"
                           "0d1289\n"
                           "0o1267\n"
                           "0b10110001\n"
                           "4509\n"
                           "00098\n";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ("0x12AB", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericHEXADECIMAL, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ("0d1289", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Column);
    
    EXPECT_STREQ("0o1267", tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericOCTAL, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(3), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Column);
    
    EXPECT_STREQ("0b10110001", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericBINARY, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(4), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Column);
    
    EXPECT_STREQ("4509", tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(5), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[4].Column);
    
    EXPECT_STREQ("00098", tokens[5].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[5].Type);
    EXPECT_EQ(static_cast<size_t>(6), tokens[5].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[5].Column);
}

TEST(TestLexer, EvaluateNumericLiteralsWithModifier)
{
    const string program = "0xFFFF.LOW\n"
                           "0d456.HIGH\n"
                           "0o66511[49]\n"
                           "0b01110101.HIGH\n"
                           "23[0x0001]\n"
                           "-0x91\n"
                           "+0o655\n";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ("0xFFFF", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericHEXADECIMAL, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ(Lexemes::OperatorDOT.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorDOT, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[1].Column);
    
    EXPECT_STREQ(Lexemes::KeywordLOW.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordLOW, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(8), tokens[2].Column);
    
    EXPECT_STREQ("0d456", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Column);
    
    EXPECT_STREQ(Lexemes::OperatorDOT.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorDOT, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(6), tokens[4].Column);
    
    EXPECT_STREQ(Lexemes::KeywordHIGH.c_str(), tokens[5].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordHIGH, tokens[5].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[5].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[5].Column);
    
    EXPECT_STREQ("0o66511", tokens[6].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericOCTAL, tokens[6].Type);
    EXPECT_EQ(static_cast<size_t>(3), tokens[6].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[6].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorOPENBRACKETS.c_str(), tokens[7].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENBRACKETS, tokens[7].Type);
    EXPECT_EQ(static_cast<size_t>(3), tokens[7].Line);
    EXPECT_EQ(static_cast<size_t>(8), tokens[7].Column);

    EXPECT_STREQ("49", tokens[8].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[8].Type);
    EXPECT_EQ(static_cast<size_t>(3), tokens[8].Line);
    EXPECT_EQ(static_cast<size_t>(9), tokens[8].Column);

    EXPECT_STREQ(Lexemes::SeparatorCLOSEBRACKETS.c_str(), tokens[9].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSEBRACKETS, tokens[9].Type);
    EXPECT_EQ(static_cast<size_t>(3), tokens[9].Line);
    EXPECT_EQ(static_cast<size_t>(11), tokens[9].Column);

    EXPECT_STREQ("0b01110101", tokens[10].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericBINARY, tokens[10].Type);
    EXPECT_EQ(static_cast<size_t>(4), tokens[10].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[10].Column);
    
    EXPECT_STREQ(Lexemes::OperatorDOT.c_str(), tokens[11].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorDOT, tokens[11].Type);
    EXPECT_EQ(static_cast<size_t>(4), tokens[11].Line);
    EXPECT_EQ(static_cast<size_t>(11), tokens[11].Column);
    
    EXPECT_STREQ(Lexemes::KeywordHIGH.c_str(), tokens[12].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordHIGH, tokens[12].Type);
    EXPECT_EQ(static_cast<size_t>(4), tokens[12].Line);
    EXPECT_EQ(static_cast<size_t>(12), tokens[12].Column);
    
    EXPECT_STREQ("23", tokens[13].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[13].Type);
    EXPECT_EQ(static_cast<size_t>(5), tokens[13].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[13].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorOPENBRACKETS.c_str(), tokens[14].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENBRACKETS, tokens[14].Type);
    EXPECT_EQ(static_cast<size_t>(5), tokens[14].Line);
    EXPECT_EQ(static_cast<size_t>(3), tokens[14].Column);
    
    EXPECT_STREQ("0x0001", tokens[15].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericHEXADECIMAL, tokens[15].Type);
    EXPECT_EQ(static_cast<size_t>(5), tokens[15].Line);
    EXPECT_EQ(static_cast<size_t>(4), tokens[15].Column);

    EXPECT_STREQ(Lexemes::SeparatorCLOSEBRACKETS.c_str(), tokens[16].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSEBRACKETS, tokens[16].Type);
    EXPECT_EQ(static_cast<size_t>(5), tokens[16].Line);
    EXPECT_EQ(static_cast<size_t>(10), tokens[16].Column);

    EXPECT_STREQ(Lexemes::OperatorMINUS.c_str(), tokens[17].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorMINUS, tokens[17].Type);
    EXPECT_EQ(static_cast<size_t>(6), tokens[17].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[17].Column);
    
    EXPECT_STREQ("0x91", tokens[18].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericHEXADECIMAL, tokens[18].Type);
    EXPECT_EQ(static_cast<size_t>(6), tokens[18].Line);
    EXPECT_EQ(static_cast<size_t>(2), tokens[18].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[19].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[19].Type);
    EXPECT_EQ(static_cast<size_t>(7), tokens[19].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[19].Column);
    
    EXPECT_STREQ("0o655", tokens[20].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericOCTAL, tokens[20].Type);
    EXPECT_EQ(static_cast<size_t>(7), tokens[20].Line);
    EXPECT_EQ(static_cast<size_t>(2), tokens[20].Column);
}

TEST(TestLexer, InvalidHexadecimalLiterals)
{
    const string number1 = "0x889y";
    const string number2 = "F7AA"; // This is an IDENTIFIER (variable name etc.);
    const string number3 = "0dFF41";
    const string number4 = "0oA5001";
    const string number5 = "0b987FA";
    const string number6 = "84FF,"; // Note that , will not cause an error.

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(number1); }, 
                      LexerException, 
                      "Invalid character 'y' near numeric literal '0x889y'");
    
    // No Exception Expected (not for number, This will be considered an identifier)
    lexer->Tokenize(number2);

    ASSERT_EXCEPTION( { lexer->Tokenize(number3); }, 
                      LexerException, 
                      "Invalid decimal numeric literal 'FF41'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number4); }, 
                      LexerException, 
                      "Invalid octal numeric literal 'A5001'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number5); }, 
                      LexerException, 
                      "Invalid binary numeric literal '987FA'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number6); }, 
                      LexerException, 
                      "Invalid decimal numeric literal '84FF'");
}

TEST(TestLexer, InvalidDecimalLiterals)
{
    const string number1 = "0d889Tj";
    const string number2 = "231p";
    const string number3 = "0dFF51";
    const string number4 = "12FF51";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(number1); }, 
                      LexerException, 
                      "Invalid character 'T' near numeric literal '0d889Tj'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number2); }, 
                      LexerException, 
                      "Invalid character 'p' near numeric literal '231p'");

    ASSERT_EXCEPTION( { lexer->Tokenize(number3); }, 
                      LexerException, 
                      "Invalid decimal numeric literal 'FF51'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number4); }, 
                      LexerException, 
                      "Invalid decimal numeric literal '12FF51'");
}

TEST(TestLexer, InvalidOctalLiterals)
{
    const string number1 = "0o4511k";
    const string number2 = "0o99999";
    const string number3 = "0oABCDEF";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(number1); }, 
                      LexerException, 
                      "Invalid character 'k' near numeric literal '0o4511k'");
                      
    ASSERT_EXCEPTION( { lexer->Tokenize(number2); }, 
                      LexerException, 
                      "Invalid octal numeric literal '99999'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number3); }, 
                      LexerException, 
                      "Invalid octal numeric literal 'ABCDEF'");
}

TEST(TestLexer, InvalidBinaryLiterals)
{
    const string number1 = "0b11110877r";
    const string number2 = "0b23456789";
    const string number3 = "0bABCDEF";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(number1); }, 
                      LexerException, 
                      "Invalid character 'r' near numeric literal '0b11110877r'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number2); }, 
                      LexerException, 
                      "Invalid binary numeric literal '23456789'");
    
    ASSERT_EXCEPTION( { lexer->Tokenize(number3); }, 
                      LexerException, 
                      "Invalid binary numeric literal 'ABCDEF'");
}

TEST(TestLexer, EvaluateBooleanLiterals)
{
    const string program = "TRUE\n"
                           "FALSE\n";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::LiteralBooleanTRUE.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralBooleanTRUE, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ(Lexemes::LiteralBooleanFALSE.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralBooleanFALSE, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Column);
}

TEST(TestLexer, EvaluateStringLiteral)
{
    const string string = "\"basic-test\"";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\"basic-test\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateStringLiteral2)
{
    const string string = "\" basic test \"";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\" basic test \"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
}

TEST(TestLexer, EvaluateStringLiteral3)
{
    const string string = "\"\nbasic\ntest\n\"";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\"\nbasic\ntest\n\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateStringLiteral4)
{
    const string string = "CONST MY_STRING {\"my-string\"}";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(5), tokens.size());
    EXPECT_STREQ(Lexemes::KeywordCONST.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordCONST, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ("MY_STRING", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[1].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorOPENCURLYBRACKETS.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENCURLYBRACKETS, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(17), tokens[2].Column);
    
    EXPECT_STREQ("\"my-string\"", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(18), tokens[3].Column);

    EXPECT_STREQ(Lexemes::SeparatorCLOSECURLYBRACKETS.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSECURLYBRACKETS, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(29), tokens[4].Column);
}

TEST(TestLexer, EvaluateStringLiteral5)
{
    const string string = "PACK\n\"\tmessy   string\"(A + 1)";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(7), tokens.size());
    
    EXPECT_STREQ(Lexemes::KeywordPACK.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::KeywordPACK, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ("\"\tmessy   string\"", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorOPENPARENTHESIS.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENPARENTHESIS, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(18), tokens[2].Column);
    
    EXPECT_STREQ("A", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(19), tokens[3].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(21), tokens[4].Column);
    
    EXPECT_STREQ("1", tokens[5].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[5].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[5].Line);
    EXPECT_EQ(static_cast<size_t>(23), tokens[5].Column);

    EXPECT_STREQ(Lexemes::SeparatorCLOSEPARENTHESIS.c_str(), tokens[6].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSEPARENTHESIS, tokens[6].Type);
    EXPECT_EQ(static_cast<size_t>(2), tokens[6].Line);
    EXPECT_EQ(static_cast<size_t>(24), tokens[6].Column);
}

TEST(TestLexer, EvaluateStringLiteral6)
{
    const string string = "\"\"";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\"\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateStringLiteral7)
{
    const string string = "\n\n\n\n\n\n\n\n\n\"sfjfashkjfhdjkfshdjkdashfkajshfjasfhakjsdhfakjsfhajskfhsjkdfheiwuyuscbvmrijeurhaBSXASJDNEN\"";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\"sfjfashkjfhdjkfshdjkdashfkajshfjasfhakjsdhfakjsfhajskfhsjkdfheiwuyuscbvmrijeurhaBSXASJDNEN\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(10), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
}

TEST(TestLexer, EvaluateStringLiteral8)
{
    const string text = "\"basic test \"";
    const string text2 = "\" basic test\"";
    const string text3 = "\"basic test\"";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(text);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\"basic test \"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    lexer->Tokenize(text2);
    tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\" basic test\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);

    lexer->Tokenize(text3);
    tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(1), tokens.size());
    EXPECT_STREQ("\"basic test\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);   
}

TEST(TestLexer, EvaluateStringLiteral9)
{
    const string string = "0x67{\"\":}";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(5), tokens.size());
    
    EXPECT_STREQ("0x67", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericHEXADECIMAL, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorOPENCURLYBRACKETS.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorOPENCURLYBRACKETS, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[1].Column);
    
    EXPECT_STREQ("\"\"", tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(6), tokens[2].Column);
    
    EXPECT_STREQ(Lexemes::OperatorSEMICOLON.c_str(), tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorSEMICOLON, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(8), tokens[3].Column);

    EXPECT_STREQ(Lexemes::SeparatorCLOSECURLYBRACKETS.c_str(), tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCLOSECURLYBRACKETS, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(9), tokens[4].Column);
}


TEST(TestLexer, EvaluateStringLiteral10)
{
    const string string = "\"string1\" + \"\" + \"string2\"";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(5), tokens.size());
    
    EXPECT_STREQ("\"string1\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(11), tokens[1].Column);
    
    EXPECT_STREQ("\"\"", tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(13), tokens[2].Column);
    
    EXPECT_STREQ(Lexemes::OperatorPLUS.c_str(), tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::OperatorPLUS, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(16), tokens[3].Column);

    EXPECT_STREQ("\"string2\"", tokens[4].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[4].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[4].Line);
    EXPECT_EQ(static_cast<size_t>(18), tokens[4].Column);
}

TEST(TestLexer, EvaluateStringLiteral11)
{
    const string string = "\"string1\"\"string2\"";
    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(string);
    auto tokens = lexer->Tokens();

    EXPECT_EQ(static_cast<size_t>(2), tokens.size());
    
    EXPECT_STREQ("\"string1\"", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);

    EXPECT_STREQ("\"string2\"", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralSTRING, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(10), tokens[1].Column);
}

TEST(TestLexer, EvaluateUnterminatedString)
{
    const string string = "\"never-ending string";
    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(string); }, 
                      LexerException, 
                      "Non-terminated string literal found");
}

TEST(TestLexer, EvaluateUnterminatedString1)
{
    const string string = " IF \n \"never-ending string PACK \" \n \" Another string";
    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(string); }, 
                      LexerException, 
                      "Non-terminated string literal found");
}

TEST(TestLexer, EvaluateCharLiteral)
{
    const string program = "'a' 'b' 'c' 'd'";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ("'a'", tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralCHAR, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ("'b'", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralCHAR, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[1].Column);
    
    EXPECT_STREQ("'c'", tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralCHAR, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(9), tokens[2].Column);
    
    EXPECT_STREQ("'d'", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralCHAR, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(13), tokens[3].Column);
}

TEST(TestLexer, EvaluateNotClosedCharLiteral)
{
    const string program = "'a' '";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(program); }, 
                      LexerException, 
                      "Non-terminated char literal found");
}

TEST(TestLexer, EvaluateInvalidCharLiteral)
{
    const string program = "'abcdefgh'";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(program); }, 
                      LexerException, 
                      "invalid char literal found ('abcdefgh')");
}

TEST(TestLexer, EvaluateInvalidCharLiteral2)
{
    const string program = "' a '";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(program); }, 
                      LexerException, 
                      "Non-terminated char literal found");
}

TEST(TestLexer, EvaluateInvalidCharLiteral3)
{
    const string program = "'\\ra'";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(program); }, 
                      LexerException, 
                      "invalid char literal found ('\\ra')");
}

TEST(TestLexer, EvaluateValidCharLiteral)
{
    const string program = "'\\s' '!' '\\\"' '#'"
                           "'$' '%' '&' '\\''"
                           "'(' ')' '*' '+'"
                           "',' '-' '.' '/'"
                           "'0' '1' '2' '3'"
                           "'4' '5' '6' '7'"
                           "'8' '9' ':' ';'"
                           "'<' '=' '>' '?'"
                           "'@' 'A' 'B' 'C'"
                           "'D' 'E' 'F' 'G'"
                           "'H' 'I' 'J' 'K'"
                           "'L' 'M' 'N' 'O'"
                           "'P' 'Q' 'R' 'S'"
                           "'T' 'U' 'V' 'W'"
                           "'X' 'Y' 'Z' '['"
                           "']' '\\' '^' '_'"
                           "'`' 'a' 'b' 'c'"
                           "'d' 'e' 'f' 'g'"
                           "'h' 'i' 'j' 'k'"
                           "'l' 'm' 'n' 'o'"
                           "'p' 'q' 'r' 's'"
                           "'t' 'u' 'v' 'w'"
                           "'x' 'y' 'z' '{'"
                           "'|' '}' '~' '\\n'"
                           "'\\a' '\\b' '\\f' '\\r'"
                           "'\\t' '\\v'";

    auto expectedChars = 
    { 
        "' '",  "'!'",  "'\"'",  "'#'",
        "'$'",  "'%'",  "'&'",  "'''",
        "'('",  "')'",  "'*'",  "'+'",
        "','",  "'-'",  "'.'",  "'/'",
        "'0'",  "'1'",  "'2'",  "'3'",
        "'4'",  "'5'",  "'6'",  "'7'",
        "'8'",  "'9'",  "':'",  "';'",
        "'<'",  "'='",  "'>'",  "'?'",
        "'@'",  "'A'",  "'B'",  "'C'",
        "'D'",  "'E'",  "'F'",  "'G'",
        "'H'",  "'I'",  "'J'",  "'K'",
        "'L'",  "'M'",  "'N'",  "'O'",
        "'P'",  "'Q'",  "'R'",  "'S'",
        "'T'",  "'U'",  "'V'",  "'W'",
        "'X'",  "'Y'",  "'Z'",  "'['",
        "']'",  "'\\'",  "'^'",  "'_'",
        "'`'",  "'a'",  "'b'",  "'c'",
        "'d'",  "'e'",  "'f'",  "'g'",
        "'h'",  "'i'",  "'j'",  "'k'",
        "'l'",  "'m'",  "'n'",  "'o'",
        "'p'",  "'q'",  "'r'",  "'s'",
        "'t'",  "'u'",  "'v'",  "'w'",
        "'x'",  "'y'",  "'z'",  "'{'",
        "'|'",  "'}'",  "'~'",  "'\n'",
        "'\a'",  "'\b'",  "'\f'",  "'\r'",
        "'\t'",  "'\v'"
        };

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    auto tokenCounter = 0;
    for (auto token : tokens)
    {
        EXPECT_STREQ(*(begin(expectedChars) + tokenCounter++), token.Lexeme.c_str());
        EXPECT_EQ(TokenType::LiteralCHAR, token.Type);
    }
}

TEST(TestLexer, InvalidScapedCharLiterals)
{
    const string sequence1 = "'\\w'";
    const string sequence2 = "'\\0'";
    const string sequence3 = "'±'";
    const string sequence4 = "'€'";

    auto lexer = make_shared<Lexer>();

    ASSERT_EXCEPTION( { lexer->Tokenize(sequence1); }, 
                      LexerException, 
                      "Unknown char literal '\\w'");

    ASSERT_EXCEPTION( { lexer->Tokenize(sequence2); }, 
                      LexerException, 
                      "Unknown char literal '\\0'");

    ASSERT_EXCEPTION( { lexer->Tokenize(sequence3); }, 
                      LexerException, 
                      "Unknown char literal '\xC2\xB1'");

    ASSERT_EXCEPTION( { lexer->Tokenize(sequence4); }, 
                      LexerException, 
                      "invalid char literal found ('€')");
}

TEST(TestLexer, EvaluateAllInstructionMnemonics)
{
    const string program = "NOP\n"
                           "LD\n"
                           "INC\n"
                           "DEC\n"
                           "RLCA\n"
                           "ADD\n"
                           "RRCA\n"
                           "STOP\n"
                           "RLA\n"
                           "JR\n"
                           "RRA\n"
                           "DAA\n"
                           "CPL\n"
                           "SCF\n"
                           "CCF\n"
                           "HALT\n"
                           "ADC\n"
                           "SUB\n"
                           "SBC\n"
                           "AND\n"
                           "XOR\n"
                           "OR\n"
                           "CP\n"
                           "RET\n"
                           "POP\n"
                           "JP\n"
                           "PUSH\n"
                           "CALL\n"
                           "RST\n"
                           "EI\n"
                           "DI\n"
                           "RLC\n"
                           "RRC\n"
                           "RL\n"
                           "RR\n"
                           "SLA\n"
                           "SRA\n"
                           "SWAP\n"
                           "SRL\n"
                           "BIT\n"
                           "RES\n"
                           "SET\n";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(program);
    auto tokens = lexer->Tokens();

    auto mnemonicString = {Lexemes::InstructionMnemonicNOP, Lexemes::InstructionMnemonicLD, Lexemes::InstructionMnemonicINC,
                           Lexemes::InstructionMnemonicDEC, Lexemes::InstructionMnemonicRLCA, Lexemes::InstructionMnemonicADD,
                           Lexemes::InstructionMnemonicRRCA, Lexemes::InstructionMnemonicSTOP, Lexemes::InstructionMnemonicRLA,
                           Lexemes::InstructionMnemonicJR, Lexemes::InstructionMnemonicRRA, Lexemes::InstructionMnemonicDAA,
                           Lexemes::InstructionMnemonicCPL, Lexemes::InstructionMnemonicSCF, Lexemes::InstructionMnemonicCCF,
                           Lexemes::InstructionMnemonicHALT, Lexemes::InstructionMnemonicADC, Lexemes::InstructionMnemonicSUB,
                           Lexemes::InstructionMnemonicSBC, Lexemes::InstructionMnemonicAND, Lexemes::InstructionMnemonicXOR,
                           Lexemes::InstructionMnemonicOR, Lexemes::InstructionMnemonicCP, Lexemes::InstructionMnemonicRET,
                           Lexemes::InstructionMnemonicPOP, Lexemes::InstructionMnemonicJP, Lexemes::InstructionMnemonicPUSH, 
                           Lexemes::InstructionMnemonicCALL, Lexemes::InstructionMnemonicRST, Lexemes::InstructionMnemonicEI,
                           Lexemes::InstructionMnemonicDI, Lexemes::InstructionMnemonicRLC, Lexemes::InstructionMnemonicRRC,
                           Lexemes::InstructionMnemonicRL, Lexemes::InstructionMnemonicRR, Lexemes::InstructionMnemonicSLA,
                           Lexemes::InstructionMnemonicSRA, Lexemes::InstructionMnemonicSWAP, Lexemes::InstructionMnemonicSRL,
                           Lexemes::InstructionMnemonicBIT, Lexemes::InstructionMnemonicRES, Lexemes::InstructionMnemonicSET};

    auto mnemonicToken = {TokenType::InstructionMnemonicNOP, TokenType::InstructionMnemonicLD, TokenType::InstructionMnemonicINC, 
                          TokenType::InstructionMnemonicDEC, TokenType::InstructionMnemonicRLCA, TokenType::InstructionMnemonicADD,
                          TokenType::InstructionMnemonicRRCA, TokenType::InstructionMnemonicSTOP, TokenType::InstructionMnemonicRLA, 
                          TokenType::InstructionMnemonicJR, TokenType::InstructionMnemonicRRA, TokenType::InstructionMnemonicDAA,
                          TokenType::InstructionMnemonicCPL, TokenType::InstructionMnemonicSCF, TokenType::InstructionMnemonicCCF,
                          TokenType::InstructionMnemonicHALT, TokenType::InstructionMnemonicADC, TokenType::InstructionMnemonicSUB, 
                          TokenType::InstructionMnemonicSBC, TokenType::InstructionMnemonicAND, TokenType::InstructionMnemonicXOR,
                          TokenType::InstructionMnemonicOR, TokenType::InstructionMnemonicCP, TokenType::InstructionMnemonicRET,
                          TokenType::InstructionMnemonicPOP, TokenType::InstructionMnemonicJP, TokenType::InstructionMnemonicPUSH, 
                          TokenType::InstructionMnemonicCALL, TokenType::InstructionMnemonicRST, TokenType::InstructionMnemonicEI,
                          TokenType::InstructionMnemonicDI, TokenType::InstructionMnemonicRLC, TokenType::InstructionMnemonicRRC,
                          TokenType::InstructionMnemonicRL, TokenType::InstructionMnemonicRR, TokenType::InstructionMnemonicSLA,
                          TokenType::InstructionMnemonicSRA, TokenType::InstructionMnemonicSWAP, TokenType::InstructionMnemonicSRL,
                          TokenType::InstructionMnemonicBIT, TokenType::InstructionMnemonicRES, TokenType::InstructionMnemonicSET};

    auto counter = 0;
    for (auto i = static_cast<size_t>(0); i < mnemonicString.size(); ++i)
    {
        EXPECT_STREQ((*(begin(mnemonicString) + i)).c_str(), tokens[counter].Lexeme.c_str());
        EXPECT_EQ(*(begin(mnemonicToken) + i), tokens[counter].Type);
        EXPECT_EQ(static_cast<size_t>(counter + 1), tokens[counter].Line);
        EXPECT_EQ(static_cast<size_t>(1), tokens[counter++].Column);
    }
}

TEST(TestLexer, SimpleInstructionTest)
{
    const string instruction = "LD A, 0xFF";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(instruction);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::InstructionMnemonicLD.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::InstructionMnemonicLD, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ("A", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(4), tokens[1].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorCOMMA.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCOMMA, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[2].Column);
    
    EXPECT_STREQ("0xFF", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericHEXADECIMAL, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(7), tokens[3].Column);
}

TEST(TestLexer, SimpleInstructionTest2)
{
    const string instruction = "ADD A_REGISTER_ALIAS,54";

    auto lexer = make_shared<Lexer>();
    lexer->Tokenize(instruction);
    auto tokens = lexer->Tokens();

    EXPECT_STREQ(Lexemes::InstructionMnemonicADD.c_str(), tokens[0].Lexeme.c_str());
    EXPECT_EQ(TokenType::InstructionMnemonicADD, tokens[0].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Line);
    EXPECT_EQ(static_cast<size_t>(1), tokens[0].Column);
    
    EXPECT_STREQ("A_REGISTER_ALIAS", tokens[1].Lexeme.c_str());
    EXPECT_EQ(TokenType::Identifier, tokens[1].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[1].Line);
    EXPECT_EQ(static_cast<size_t>(5), tokens[1].Column);
    
    EXPECT_STREQ(Lexemes::SeparatorCOMMA.c_str(), tokens[2].Lexeme.c_str());
    EXPECT_EQ(TokenType::SeparatorCOMMA, tokens[2].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[2].Line);
    EXPECT_EQ(static_cast<size_t>(21), tokens[2].Column);
    
    EXPECT_STREQ("54", tokens[3].Lexeme.c_str());
    EXPECT_EQ(TokenType::LiteralNumericDECIMAL, tokens[3].Type);
    EXPECT_EQ(static_cast<size_t>(1), tokens[3].Line);
    EXPECT_EQ(static_cast<size_t>(22), tokens[3].Column);
}