#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <utility>

#include "LexicalAnalyzer.h"
#include "Lexemes.h"
#include "FuncSyntacticAnalyzer.h"
#include "Construction.h"
#include "GBXAsmExceptions.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::frontend::parsers;
using namespace gbxasm::utilities;
using namespace std;

TEST(AssemblerTests_FUNCSyntacticAnalyzer, SanityCheckFuncTokenization)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "BGN\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS MY_TYPE\n"
                        "\tIN: INPUT_4 AS MY_ARRAY_TYPE[10]\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tOUT: OUTPUT_3 AS WORD[0b10]\n"
                        "\tLD A, 0xFF\n"
                        "\tLD OUTPUT_1, A\n"
                        "\tLD OUTPUT_2, TRUE\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    lexer->Tokenize(func);
    auto tokens = lexer->Tokens();

    auto tokenTypes = { // FUNC MY_FUNCTION
                        TokenType::KeywordFUNC, TokenType::Identifier,
                        // BGN
                        TokenType::KeywordBGN,
                        // IN: INPUT_1
                        TokenType::KeywordIN, TokenType::OperatorSEMICOLON, TokenType::Identifier,
                        // IN: INPUT_2 AS BYTE
                        TokenType::KeywordIN, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::KeywordBYTE,
                        // IN: INPUT_3 AS WORD
                        TokenType::KeywordIN, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::Identifier,
                        // IN: INPUT_4 AS MY_ARRAY_TYPE[10]
                        TokenType::KeywordIN, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::Identifier,
                        TokenType::SeparatorOPENBRACKETS, TokenType::LiteralNumericDECIMAL, TokenType::SeparatorCLOSEBRACKETS,
                        // OUT: OUTPUT_1
                        TokenType::KeywordOUT, TokenType::OperatorSEMICOLON, TokenType::Identifier,
                        // OUT: OUTPUT_2 AS BOOL
                        TokenType::KeywordOUT, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::KeywordBOOL,
                        // OUT: OUTPUT_3 AS WORD[0b10]
                        TokenType::KeywordOUT, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::KeywordWORD,
                        TokenType::SeparatorOPENBRACKETS, TokenType::LiteralNumericBINARY, TokenType::SeparatorCLOSEBRACKETS,
                        // LD A, 0xFF
                        TokenType::InstructionMnemonicLD, TokenType::Identifier, TokenType::SeparatorCOMMA, TokenType::LiteralNumericHEXADECIMAL,
                        // LD OUTPUT_1, A
                        TokenType::InstructionMnemonicLD, TokenType::Identifier, TokenType::SeparatorCOMMA, TokenType::Identifier,
                        // LD OUTPUT_2, TRUE
                        TokenType::InstructionMnemonicLD, TokenType::Identifier, TokenType::SeparatorCOMMA, TokenType::LiteralBooleanTRUE,
                        // END
                        TokenType::KeywordEND
                      };

    auto lexemes = { // FUNC MY_FUNCTION
                        Lexemes::KeywordFUNC.c_str(), "MY_FUNCTION",
                        // BGN
                        Lexemes::KeywordBGN.c_str(),
                        // IN: INPUT_1
                        Lexemes::KeywordIN.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "INPUT_1",
                        // IN: INPUT_2 AS BYTE
                        Lexemes::KeywordIN.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "INPUT_2", Lexemes::KeywordAS.c_str(), Lexemes::KeywordBYTE.c_str(),
                        // IN: INPUT_3 AS WORD
                        Lexemes::KeywordIN.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "INPUT_3", Lexemes::KeywordAS.c_str(), "MY_TYPE",
                        // IN: INPUT_4 AS MY_ARRAY_TYPE[10]
                        Lexemes::KeywordIN.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "INPUT_4", Lexemes::KeywordAS.c_str(), "MY_ARRAY_TYPE",
                        Lexemes::SeparatorOPENBRACKETS.c_str(), "10", Lexemes::SeparatorCLOSEBRACKETS.c_str(),
                        // OUT: OUTPUT_1
                        Lexemes::KeywordOUT.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "OUTPUT_1",
                        // OUT: OUTPUT_2 AS BOOL
                        Lexemes::KeywordOUT.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "OUTPUT_2", Lexemes::KeywordAS.c_str(), Lexemes::KeywordBOOL.c_str(),
                        // OUT: OUTPUT_3 AS WORD[0b10]
                        Lexemes::KeywordOUT.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "OUTPUT_3", Lexemes::KeywordAS.c_str(), Lexemes::KeywordWORD.c_str(),
                        Lexemes::SeparatorOPENBRACKETS.c_str(), "0b10", Lexemes::SeparatorCLOSEBRACKETS.c_str(),
                        // LD A, 0xFF
                        Lexemes::InstructionMnemonicLD.c_str(), "A", Lexemes::SeparatorCOMMA.c_str(), "0xFF",
                        // LD OUTPUT_1, A
                        Lexemes::InstructionMnemonicLD.c_str(), "OUTPUT_1", Lexemes::SeparatorCOMMA.c_str(), "A",
                        // LD OUTPUT_2, TRUE
                        Lexemes::InstructionMnemonicLD.c_str(), "OUTPUT_2", Lexemes::SeparatorCOMMA.c_str(), Lexemes::LiteralBooleanTRUE.c_str(),
                        // END
                        Lexemes::KeywordEND.c_str()
                      };

    auto columns = { // FUNC MY_FUNCTION
                     1llu, 6llu,
                     // BGN
                     1llu,
                     // IN: INPUT_1
                     2llu, 4llu, 6llu,
                     // IN: INPUT_2 AS BYTE
                     2llu, 4llu, 6llu, 14llu, 17llu,
                     // IN: INPUT_3 AS WORD
                     2llu, 4llu, 6llu, 14llu, 17llu,
                     // IN: INPUT_4 AS MY_ARRAY_TYPE[10]
                     2llu, 4llu, 6llu, 14llu, 17llu, 30llu, 31llu, 33llu,
                     // OUT: OUTPUT_1
                     2llu, 5llu, 7llu,
                     // OUT: OUTPUT_2 AS BOOL
                     2llu, 5llu, 7llu, 16llu, 19llu,
                     // OUT: OUTPUT_3 AS WORD[0b10]
                     2llu, 5llu, 7llu, 16llu, 19llu, 23llu, 24llu, 28llu,
                     // LD A, 0xFF
                     2llu, 5llu, 6llu, 8llu,
                     // LD OUTPUT_1, A
                     2llu, 5llu, 13llu, 15llu,
                     // LD OUTPUT_2, TRUE
                     2llu, 5llu, 13llu, 15llu,
                     // END
                     1llu,
                   };
    
    auto lines = {   // FUNC MY_FUNCTION
                     1llu, 1llu,
                     // BGN
                     2llu,
                     // IN: INPUT_1
                     3llu, 3llu, 3llu,
                     // IN: INPUT_2 AS BYTE
                     4llu, 4llu, 4llu, 4llu, 4llu,
                     // IN: INPUT_3 AS WORD
                     5llu, 5llu, 5llu, 5llu, 5llu,
                     // IN: INPUT_4 AS MY_ARRAY_TYPE[10]
                     6llu, 6llu, 6llu, 6llu, 6llu, 6llu, 6llu, 6llu,
                     // OUT: OUTPUT_1
                     7llu, 7llu, 7llu,
                     // OUT: OUTPUT_2 AS BOOL
                     8llu, 8llu, 8llu, 8llu, 8llu,
                     // OUT: OUTPUT_3 AS WORD[0b10]
                     9llu, 9llu, 9llu, 9llu, 9llu, 9llu, 9llu, 9llu,
                     // LD A, 0xFF
                     10llu, 10llu, 10llu, 10llu,
                     // LD OUTPUT_1, A
                     11llu, 11llu, 11llu, 11llu,
                     // LD OUTPUT_2, TRUE
                     12llu, 12llu, 12llu, 12llu,
                     // END
                     13llu,
                   };


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseBaseFunc)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "BGN\n"
                        "\tLD A, 0xFF\n"
                        "\tLD OUTPUT_1, A\n"
                        "\tLD OUTPUT_2, TRUE\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseBaseFuncNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "BGN\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWitOneInputArgument)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "BGN\n"
                        "\tLD A, 0xFF\n"
                        "\tLD OUTPUT_1, A\n"
                        "\tLD OUTPUT_2, TRUE\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMultipleInputArguments)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2\n"
                        "BGN\n"
                        "\tLD A, 0xFF\n"
                        "\tLD OUTPUT_1, A\n"
                        "\tLD OUTPUT_2, TRUE\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMultipleInputArgumentNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2\n"
                        "BGN\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMultipleInputArgument2)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2\n"
                        "\tIN: INPUT_3\n"
                        "\tIN: INPUT_4\n"
                        "\tIN: INPUT_5\n"
                        "BGN\n"
                        "\tLD A, 0xFF\n"
                        "\tLD OUTPUT_1, A\n"
                        "\tLD OUTPUT_2, TRUE\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMultipleInputArgument2NoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2\n"
                        "\tIN: INPUT_3\n"
                        "\tIN: INPUT_4\n"
                        "\tIN: INPUT_5\n"
                        "BGN\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

/*
TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalIfBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tIF A != 0x00 THEN\n"
                        "\t\tLD OUTPUT_1, A\n"
                        "\tELSE\n"
                        "\t\tTHRW 0x99\n"
                        "\tEND\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalPackBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tPACK MY_PACK\n"
                        "\t\tBYTE FIELD_1\n"
                        "\t\tBYTE FIELD_2[10]\n"
                        "\tEND\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalWhenBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tWHEN MY_VARIABLE\n"
                        "\t\tIS 0x00:\n"
                        "\t\t\tLD A, 0x00\n"
                        "\t\t\tBRK\n"
                        "\t\tIS 0x01:\n"
                        "\t\t\tLD A, 0x01\n"
                        "\t\t\tBRK\n"
                        "\t\tDFLT:\n"
                        "\t\t\tLD A, 0xFF\n"
                        "\t\t\tBRK\n"
                        "\tEND\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalReptBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tREPT FOR 0x10\n"
                        "\t\tINC A\n"
                        "\tEND\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalTryBlock)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tTRY\n"
                        "\t\tLD A, 0x00\n"
                        "\tEXPT\n"
                        "\t\tRET\n"
                        "\tEND\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalFuncBlock)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tFUNC INNER_FUNC\n"
                        "\t\tIN: INNER_INPUT_1\n"
                        "\t\tOUT: INNER_OUTPUT_1\n"
                        "\t\tRET\n"
                        "\tEND\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    auto parser = make_shared<FuncSyntacticAnalyzer>();
    lexer->Tokenize(func);
    auto currentToken = begin(lexer->Tokens());
    auto endIterator = end(lexer->Tokens());
    parser->TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser->IsAccepted());
}*/