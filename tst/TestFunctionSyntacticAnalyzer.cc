#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <utility>

#include "../src/frontend/LexicalAnalyzer.h"
#include "../src/frontend/Lexemes.h"
#include "../src/frontend/parsers/FuncSyntacticAnalyzer.h"
#include "../src/interfaces/Construction.h"
#include "../src/GBXAsmExceptions.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::frontend::parsers;
using namespace gbxasm::utilities;
using namespace std;

TEST(TestFunctionSyntacticAnalyzer, SanityCheckFuncTokenization)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS MY_TYPE\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "\tLD A, 0xFF\n"
                        "\tLD OUTPUT_1, A\n"
                        "\tLD OUTPUT_2, TRUE\n"
                        "END";

    auto lexer = make_shared<LexicalAnalyzer>();
    lexer->Tokenize(func);
    auto tokens = lexer->Tokens();

    auto tokenTypes = { // FUNC MY_FUNCTION
                        TokenType::KeywordFUNC, TokenType::Identifier,
                        // IN: INPUT_1
                        TokenType::KeywordIN, TokenType::OperatorSEMICOLON, TokenType::Identifier,
                        // IN: INPUT_2 AS BYTE
                        TokenType::KeywordIN, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::KeywordBYTE,
                        // IN: INPUT_3 AS WORD
                        TokenType::KeywordIN, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::Identifier,
                        // OUT: OUTPUT_1
                        TokenType::KeywordOUT, TokenType::OperatorSEMICOLON, TokenType::Identifier,
                        // OUT: OUTPUT_2 AS BOOL
                        TokenType::KeywordOUT, TokenType::OperatorSEMICOLON, TokenType::Identifier, TokenType::KeywordAS, TokenType::KeywordBOOL,
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
                        // IN: INPUT_1
                        Lexemes::KeywordIN.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "INPUT_1",
                        // IN: INPUT_2 AS BYTE
                        Lexemes::KeywordIN.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "INPUT_2", Lexemes::KeywordAS.c_str(), Lexemes::KeywordBYTE.c_str(),
                        // IN: INPUT_3 AS WORD
                        Lexemes::KeywordIN.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "INPUT_3", Lexemes::KeywordAS.c_str(), "MY_TYPE",
                        // OUT: OUTPUT_1
                        Lexemes::KeywordOUT.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "OUTPUT_1",
                        // OUT: OUTPUT_2 AS BOOL
                        Lexemes::KeywordOUT.c_str(), Lexemes::OperatorSEMICOLON.c_str(), "OUTPUT_2", Lexemes::KeywordAS.c_str(), Lexemes::KeywordBOOL.c_str(),
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
                     // IN: INPUT_1
                     2llu, 4llu, 6llu,
                     // IN: INPUT_2 AS BYTE
                     2llu, 4llu, 6llu, 14llu, 17llu,
                     // IN: INPUT_3 AS WORD
                     2llu, 4llu, 6llu, 14llu, 17llu,
                     // OUT: OUTPUT_1
                     2llu, 5llu, 7llu,
                     // OUT: OUTPUT_2 AS BOOL
                     2llu, 5llu, 7llu, 16llu, 19llu,
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
                     // IN: INPUT_1
                     2llu, 2llu, 2llu,
                     // IN: INPUT_2 AS BYTE
                     3llu, 3llu, 3llu, 3llu, 3llu,
                     // IN: INPUT_3 AS WORD
                     4llu, 4llu, 4llu, 4llu, 4llu,
                     // OUT: OUTPUT_1
                     5llu, 5llu, 5llu,
                     // OUT: OUTPUT_2 AS BOOL
                     6llu, 6llu, 6llu, 6llu, 6llu,
                     // LD A, 0xFF
                     7llu, 7llu, 7llu, 7llu,
                     // LD OUTPUT_1, A
                     8llu, 8llu, 8llu, 8llu,
                     // LD OUTPUT_2, TRUE
                     9llu, 9llu, 9llu, 9llu,
                     // END
                     10llu,
                   };


    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(TestFunctionSyntacticAnalyzer, ParseFunc)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
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

TEST(TestFunctionSyntacticAnalyzer, ParseFuncWithInternalBlocks)
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