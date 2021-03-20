#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "DeclaredMember.h"
#include "LexicalAnalyzer.h"
#include "Lexemes.h"
#include "FUNCIntermediateRepresentation.h"
#include "FuncSyntacticAnalyzer.h"
#include "Construction.h"
#include "GBXAsmExceptions.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::intermediate_representation;
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

    LexicalAnalyzer lexer;
    lexer.Tokenize(func);
    auto tokens = lexer.Tokens();

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

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseBaseFuncNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
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

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWitOneInputArgumentWithNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
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

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMultipleInputArgumentNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
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

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
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

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithOneOutputArgument)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tOUT: OUTPUT_1\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithOneOutputArgumentWithNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tOUT: OUTPUT_1\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMultipleOutputArgument2)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2\n"
                        "\tOUT: OUTPUT_3\n"
                        "\tOUT: OUTPUT_4\n"
                        "\tOUT: OUTPUT_5\n"
                        "\tOUT: OUTPUT_6\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMultipleOutputArgumentWithNoBody2)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2\n"
                        "\tOUT: OUTPUT_3\n"
                        "\tOUT: OUTPUT_4\n"
                        "\tOUT: OUTPUT_5\n"
                        "\tOUT: OUTPUT_6\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMixedInputAndOutputArguments)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2\n"
                        "\tIN: INPUT_3\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2\n"
                        "BGN\n"
                        "\t\tLD OUTPUT_1, A\n"
                        "\t\tTHRW 0x99\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithMixedInputAndOutputArgumentsNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2\n"
                        "\tIN: INPUT_3\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithASTypeSpecifier)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS BYTE\n"
                        "\tOUT: OUTPUT_1 AS BYTE\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithASTypeSpecifierWithNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS BYTE\n"
                        "\tOUT: OUTPUT_1 AS BYTE\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithPACKType)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS MY_TYPE_1\n"
                        "\tOUT: OUTPUT_1 AS MY_TYPE_2\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithPACKTypeNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS MY_TYPE_1\n"
                        "\tOUT: OUTPUT_1 AS MY_TYPE_2\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithMixedTypeAndDeclarationFormats)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS MY_TYPE_1\n"
                        "\tIN: INPUT_3\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS MY_TYPE_2\n"
                        "\tOUT: OUTPUT_3 AS STR\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithMixedTypeAndDeclarationFormats2)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_3 AS OTHER\n"
                        "\tIN: INPUT_2 AS DWRD\n"
                        "\tOUT: OUTPUT_2 AS _2\n"
                        "\tOUT: OUTPUT_3 AS STR\n"
                        "\tOUT: OUTPUT_1\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithASTypeSpecifierAndArrayType)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS BYTE[0x67]\n"
                        "\tOUT: OUTPUT_1 AS BYTE[0o1233]\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithASTypeSpecifierAndArrayTypeNoBody)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS BYTE[0x67]\n"
                        "\tOUT: OUTPUT_1 AS BYTE[0o1233]\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ArgumentsWithMixedTypeAndDeclarationFormats3)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS MY_TYPE_1\n"
                        "\tIN: INPUT_3\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS BYTE[90]\n"
                        "\tIN: INPUT_4 AS ANOTHER_TYPE[0b0101010]\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS MY_TYPE_2\n"
                        "\tOUT: OUTPUT_3 AS STR\n"
                        "\tOUT: OUTPUT_4 AS BOOL[100]\n"
                        "\tOUT: OUTPUT_5 AS MY_TRISTATE[0d10]\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tINC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}


TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalIfBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD[2]\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "BGN\n"
                        "\tIF A != 0x00 THEN\n"
                        "\t\tLD OUTPUT_1, A\n"
                        "\tELSE\n"
                        "\t\tTHRW 0x99\n"
                        "\tEND\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;
    
    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalPackBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS ANOTHER_TYPE[0x199]\n"
                        "BGN\n"
                        "\tPACK MY_PACK\n"
                        "\t\tBYTE FIELD_1\n"
                        "\t\tBYTE FIELD_2[10]\n"
                        "\tEND\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalWhenBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS TYPE[1000]\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "BGN"
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

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalReptBlocks)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "BGN\n"
                        "\tREPT FOR 0x10\n"
                        "\t\tINC A\n"
                        "\tEND\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalTryBlock)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS BOOL\n"
                        "BGN\n"
                        "\tTRY\n"
                        "\t\tLD A, 0x00\n"
                        "\tEXPT\n"
                        "\t\tRET\n"
                        "\tEND\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, ParseFuncWithInternalFuncBlock)
{
    const string func = "FUNC MY_FUNCTION\n"
                        "\tIN: INPUT_1 AS STR[100]\n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS WORD\n"
                        "BGN\n"
                        "\tFUNC INNER_FUNC\n"
                        "\t\tIN: INNER_INPUT_1\n"
                        "\t\tOUT: INNER_OUTPUT_1\n"
                        "\t\tRET\n"
                        "\tEND\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, FuncIntermediateRepresentation)
{
    const string func = "\nFUNC DOES_NOTHING\n"
                        "\tIN: INPUT_1 \n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS MY_TYPE_0\n"
                        "\tIN: INPUT_4 AS BOOL[0b111]\n"
                        "\tIN: INPUT_5 AS MY_TYPE_1[0o765]\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS DWRD\n"
                        "\tOUT: OUTPUT_3 AS MY_TYPE_2\n"
                        "\tOUT: OUTPUT_4 AS STR[100]\n"
                        "\tOUT: OUTPUT_5 AS MY_TYPE_3[0x80]\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tDEC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto funcRepresentation = dynamic_pointer_cast<FUNCIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, funcRepresentation);

    EXPECT_STREQ("DOES_NOTHING", string(funcRepresentation->Identifier()).c_str());
    EXPECT_EQ(2llu, funcRepresentation->Line());
    EXPECT_EQ(1llu, funcRepresentation->Column());
    EXPECT_EQ(8llu, funcRepresentation->BodyTokens().size());
    EXPECT_EQ(5llu, funcRepresentation->InputArguments().size());
    EXPECT_EQ(5llu, funcRepresentation->OutputArguments().size());

    // Check Arguments
    EXPECT_STREQ("INPUT_1", funcRepresentation->InputArguments()[0].Identifier.c_str());
    EXPECT_STREQ("INPUT_2", funcRepresentation->InputArguments()[1].Identifier.c_str());
    EXPECT_STREQ("INPUT_3", funcRepresentation->InputArguments()[2].Identifier.c_str());
    EXPECT_STREQ("INPUT_4", funcRepresentation->InputArguments()[3].Identifier.c_str());
    EXPECT_STREQ("INPUT_5", funcRepresentation->InputArguments()[4].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_1", funcRepresentation->OutputArguments()[0].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_2", funcRepresentation->OutputArguments()[1].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_3", funcRepresentation->OutputArguments()[2].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_4", funcRepresentation->OutputArguments()[3].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_5", funcRepresentation->OutputArguments()[4].Identifier.c_str());
    
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[0].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[1].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[2].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[3].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[4].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[0].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[1].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[2].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[3].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[4].Direction);
   
    EXPECT_STREQ("Unknown", funcRepresentation->InputArguments()[0].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("Byte", funcRepresentation->InputArguments()[1].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_0", funcRepresentation->InputArguments()[2].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("Bool", funcRepresentation->InputArguments()[3].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_1", funcRepresentation->InputArguments()[4].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("Unknown", funcRepresentation->OutputArguments()[0].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("DoubleWord", funcRepresentation->OutputArguments()[1].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_2", funcRepresentation->OutputArguments()[2].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("String", funcRepresentation->OutputArguments()[3].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_3", funcRepresentation->OutputArguments()[4].Type.LexicalTypeName.c_str());
    
    EXPECT_EQ(TypeName::Unknown, funcRepresentation->InputArguments()[0].Type.Name);
    EXPECT_EQ(TypeName::Byte, funcRepresentation->InputArguments()[1].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->InputArguments()[2].Type.Name);
    EXPECT_EQ(TypeName::Bool, funcRepresentation->InputArguments()[3].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->InputArguments()[4].Type.Name);
    EXPECT_EQ(TypeName::Unknown, funcRepresentation->OutputArguments()[0].Type.Name);
    EXPECT_EQ(TypeName::DoubleWord, funcRepresentation->OutputArguments()[1].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->OutputArguments()[2].Type.Name);
    EXPECT_EQ(TypeName::String, funcRepresentation->OutputArguments()[3].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->OutputArguments()[4].Type.Name);
    
    EXPECT_EQ(1llu, funcRepresentation->InputArguments()[0].Type.Size);
    EXPECT_EQ(1llu, funcRepresentation->InputArguments()[1].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->InputArguments()[2].Type.Size);
    EXPECT_EQ(1llu, funcRepresentation->InputArguments()[3].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->InputArguments()[4].Type.Size);
    EXPECT_EQ(1llu, funcRepresentation->OutputArguments()[0].Type.Size);
    EXPECT_EQ(4llu, funcRepresentation->OutputArguments()[1].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->OutputArguments()[2].Type.Size);
    EXPECT_EQ(1llu, funcRepresentation->OutputArguments()[3].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->OutputArguments()[4].Type.Size);

    EXPECT_FALSE(funcRepresentation->InputArguments()[0].IsArray);
    EXPECT_FALSE(funcRepresentation->InputArguments()[1].IsArray);
    EXPECT_FALSE(funcRepresentation->InputArguments()[2].IsArray);
    EXPECT_TRUE(funcRepresentation->InputArguments()[3].IsArray);
    EXPECT_TRUE(funcRepresentation->InputArguments()[4].IsArray);
    EXPECT_FALSE(funcRepresentation->OutputArguments()[0].IsArray);
    EXPECT_FALSE(funcRepresentation->OutputArguments()[1].IsArray);
    EXPECT_FALSE(funcRepresentation->OutputArguments()[2].IsArray);
    EXPECT_TRUE(funcRepresentation->OutputArguments()[3].IsArray);
    EXPECT_TRUE(funcRepresentation->OutputArguments()[4].IsArray);

    EXPECT_STREQ("0", funcRepresentation->InputArguments()[0].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->InputArguments()[1].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->InputArguments()[2].ArrayLength.c_str());
    EXPECT_STREQ("0b111", funcRepresentation->InputArguments()[3].ArrayLength.c_str());
    EXPECT_STREQ("0o765", funcRepresentation->InputArguments()[4].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->OutputArguments()[0].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->OutputArguments()[1].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->OutputArguments()[2].ArrayLength.c_str());
    EXPECT_STREQ("100", funcRepresentation->OutputArguments()[3].ArrayLength.c_str());
    EXPECT_STREQ("0x80", funcRepresentation->OutputArguments()[4].ArrayLength.c_str());

    // Check body content
    EXPECT_EQ(TokenType::InstructionMnemonicINC, funcRepresentation->BodyTokens()[0].Type);
    EXPECT_STREQ(Lexemes::InstructionMnemonicINC.c_str(), funcRepresentation->BodyTokens()[0].Lexeme.c_str());
    EXPECT_EQ(14llu, funcRepresentation->BodyTokens()[0].Line);
    EXPECT_EQ(2llu, funcRepresentation->BodyTokens()[0].Column);
    
    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[1].Type);
    EXPECT_STREQ("A", funcRepresentation->BodyTokens()[1].Lexeme.c_str());
    EXPECT_EQ(14llu, funcRepresentation->BodyTokens()[1].Line);
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[1].Column);
    
    EXPECT_EQ(TokenType::InstructionMnemonicDEC, funcRepresentation->BodyTokens()[2].Type);
    EXPECT_STREQ(Lexemes::InstructionMnemonicDEC.c_str(), funcRepresentation->BodyTokens()[2].Lexeme.c_str());
    EXPECT_EQ(15llu, funcRepresentation->BodyTokens()[2].Line);
    EXPECT_EQ(2llu, funcRepresentation->BodyTokens()[2].Column);
    
    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[3].Type);
    EXPECT_STREQ("A", funcRepresentation->BodyTokens()[3].Lexeme.c_str());
    EXPECT_EQ(15llu, funcRepresentation->BodyTokens()[3].Line);
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[3].Column);
    
    EXPECT_EQ(TokenType::InstructionMnemonicLD, funcRepresentation->BodyTokens()[4].Type);
    EXPECT_STREQ(Lexemes::InstructionMnemonicLD.c_str(), funcRepresentation->BodyTokens()[4].Lexeme.c_str());
    EXPECT_EQ(16llu, funcRepresentation->BodyTokens()[4].Line);
    EXPECT_EQ(2llu, funcRepresentation->BodyTokens()[4].Column);
    
    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[5].Type);
    EXPECT_STREQ("OUTPUT_1", funcRepresentation->BodyTokens()[5].Lexeme.c_str());
    EXPECT_EQ(16llu, funcRepresentation->BodyTokens()[5].Line);
    EXPECT_EQ(5llu, funcRepresentation->BodyTokens()[5].Column);
    
    EXPECT_EQ(TokenType::SeparatorCOMMA, funcRepresentation->BodyTokens()[6].Type);
    EXPECT_STREQ(Lexemes::SeparatorCOMMA.c_str(), funcRepresentation->BodyTokens()[6].Lexeme.c_str());
    EXPECT_EQ(16llu, funcRepresentation->BodyTokens()[6].Line);
    EXPECT_EQ(13llu, funcRepresentation->BodyTokens()[6].Column);

    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[7].Type);
    EXPECT_STREQ("A", funcRepresentation->BodyTokens()[7].Lexeme.c_str());
    EXPECT_EQ(16llu, funcRepresentation->BodyTokens()[7].Line);
    EXPECT_EQ(15llu, funcRepresentation->BodyTokens()[7].Column);
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, FuncIntermediateRepresentation2)
{
    const string func = "FUNC SAMPLE_FUNCTION\n"
                        "\tIN: INPUT_1 \n"
                        "\tIN: INPUT_2 AS BYTE\n"
                        "\tIN: INPUT_3 AS MY_TYPE_0\n"
                        "\tIN: INPUT_4 AS BOOL[0b111]\n"
                        "\tIN: INPUT_5 AS MY_TYPE_1[0o765]\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto funcRepresentation = dynamic_pointer_cast<FUNCIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, funcRepresentation);

    EXPECT_STREQ("SAMPLE_FUNCTION", string(funcRepresentation->Identifier()).c_str());
    EXPECT_EQ(1llu, funcRepresentation->Line());
    EXPECT_EQ(1llu, funcRepresentation->Column());
    EXPECT_EQ(0llu, funcRepresentation->BodyTokens().size());
    EXPECT_EQ(5llu, funcRepresentation->InputArguments().size());
    EXPECT_EQ(0llu, funcRepresentation->OutputArguments().size());

    // Check Arguments
    EXPECT_STREQ("INPUT_1", funcRepresentation->InputArguments()[0].Identifier.c_str());
    EXPECT_STREQ("INPUT_2", funcRepresentation->InputArguments()[1].Identifier.c_str());
    EXPECT_STREQ("INPUT_3", funcRepresentation->InputArguments()[2].Identifier.c_str());
    EXPECT_STREQ("INPUT_4", funcRepresentation->InputArguments()[3].Identifier.c_str());
    EXPECT_STREQ("INPUT_5", funcRepresentation->InputArguments()[4].Identifier.c_str());
    
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[0].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[1].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[2].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[3].Direction);
    EXPECT_EQ(ArgumentDirection::Input, funcRepresentation->InputArguments()[4].Direction);
    
    EXPECT_STREQ("Unknown", funcRepresentation->InputArguments()[0].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("Byte", funcRepresentation->InputArguments()[1].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_0", funcRepresentation->InputArguments()[2].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("Bool", funcRepresentation->InputArguments()[3].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_1", funcRepresentation->InputArguments()[4].Type.LexicalTypeName.c_str());
    
    EXPECT_EQ(TypeName::Unknown, funcRepresentation->InputArguments()[0].Type.Name);
    EXPECT_EQ(TypeName::Byte, funcRepresentation->InputArguments()[1].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->InputArguments()[2].Type.Name);
    EXPECT_EQ(TypeName::Bool, funcRepresentation->InputArguments()[3].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->InputArguments()[4].Type.Name);
    
    EXPECT_EQ(1llu, funcRepresentation->InputArguments()[0].Type.Size);
    EXPECT_EQ(1llu, funcRepresentation->InputArguments()[1].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->InputArguments()[2].Type.Size);
    EXPECT_EQ(1llu, funcRepresentation->InputArguments()[3].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->InputArguments()[4].Type.Size);

    EXPECT_FALSE(funcRepresentation->InputArguments()[0].IsArray);
    EXPECT_FALSE(funcRepresentation->InputArguments()[1].IsArray);
    EXPECT_FALSE(funcRepresentation->InputArguments()[2].IsArray);
    EXPECT_TRUE(funcRepresentation->InputArguments()[3].IsArray);
    EXPECT_TRUE(funcRepresentation->InputArguments()[4].IsArray);

    EXPECT_STREQ("0", funcRepresentation->InputArguments()[0].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->InputArguments()[1].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->InputArguments()[2].ArrayLength.c_str());
    EXPECT_STREQ("0b111", funcRepresentation->InputArguments()[3].ArrayLength.c_str());
    EXPECT_STREQ("0o765", funcRepresentation->InputArguments()[4].ArrayLength.c_str());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, FuncIntermediateRepresentation3)
{
    const string func = "\nFUNC DOES_NOTHING\n"
                        "\tOUT: OUTPUT_1\n"
                        "\tOUT: OUTPUT_2 AS DWRD\n"
                        "\tOUT: OUTPUT_3 AS MY_TYPE_2\n"
                        "\tOUT: OUTPUT_4 AS STR[100]\n"
                        "\tOUT: OUTPUT_5 AS MY_TYPE_3[0x80]\n"
                        "BGN\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto funcRepresentation = dynamic_pointer_cast<FUNCIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, funcRepresentation);

    EXPECT_STREQ("DOES_NOTHING", string(funcRepresentation->Identifier()).c_str());
    EXPECT_EQ(2llu, funcRepresentation->Line());
    EXPECT_EQ(1llu, funcRepresentation->Column());
    EXPECT_EQ(0llu, funcRepresentation->BodyTokens().size());
    EXPECT_EQ(0llu, funcRepresentation->InputArguments().size());
    EXPECT_EQ(5llu, funcRepresentation->OutputArguments().size());

    // Check Arguments
    EXPECT_STREQ("OUTPUT_1", funcRepresentation->OutputArguments()[0].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_2", funcRepresentation->OutputArguments()[1].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_3", funcRepresentation->OutputArguments()[2].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_4", funcRepresentation->OutputArguments()[3].Identifier.c_str());
    EXPECT_STREQ("OUTPUT_5", funcRepresentation->OutputArguments()[4].Identifier.c_str());
    
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[0].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[1].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[2].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[3].Direction);
    EXPECT_EQ(ArgumentDirection::Output, funcRepresentation->OutputArguments()[4].Direction);
   
    EXPECT_STREQ("Unknown", funcRepresentation->OutputArguments()[0].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("DoubleWord", funcRepresentation->OutputArguments()[1].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_2", funcRepresentation->OutputArguments()[2].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("String", funcRepresentation->OutputArguments()[3].Type.LexicalTypeName.c_str());
    EXPECT_STREQ("MY_TYPE_3", funcRepresentation->OutputArguments()[4].Type.LexicalTypeName.c_str());
    
    EXPECT_EQ(TypeName::Unknown, funcRepresentation->OutputArguments()[0].Type.Name);
    EXPECT_EQ(TypeName::DoubleWord, funcRepresentation->OutputArguments()[1].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->OutputArguments()[2].Type.Name);
    EXPECT_EQ(TypeName::String, funcRepresentation->OutputArguments()[3].Type.Name);
    EXPECT_EQ(TypeName::Custom, funcRepresentation->OutputArguments()[4].Type.Name);
    
    EXPECT_EQ(1llu, funcRepresentation->OutputArguments()[0].Type.Size);
    EXPECT_EQ(4llu, funcRepresentation->OutputArguments()[1].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->OutputArguments()[2].Type.Size);
    EXPECT_EQ(1llu, funcRepresentation->OutputArguments()[3].Type.Size);
    EXPECT_EQ(numeric_limits<size_t>::max(), funcRepresentation->OutputArguments()[4].Type.Size);

    EXPECT_FALSE(funcRepresentation->OutputArguments()[0].IsArray);
    EXPECT_FALSE(funcRepresentation->OutputArguments()[1].IsArray);
    EXPECT_FALSE(funcRepresentation->OutputArguments()[2].IsArray);
    EXPECT_TRUE(funcRepresentation->OutputArguments()[3].IsArray);
    EXPECT_TRUE(funcRepresentation->OutputArguments()[4].IsArray);

    EXPECT_STREQ("0", funcRepresentation->OutputArguments()[0].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->OutputArguments()[1].ArrayLength.c_str());
    EXPECT_STREQ("0", funcRepresentation->OutputArguments()[2].ArrayLength.c_str());
    EXPECT_STREQ("100", funcRepresentation->OutputArguments()[3].ArrayLength.c_str());
    EXPECT_STREQ("0x80", funcRepresentation->OutputArguments()[4].ArrayLength.c_str());
}

TEST(AssemblerTests_FUNCSyntacticAnalyzer, FuncIntermediateRepresentation4)
{
    const string func = "\nFUNC DOES_NOTHING\n"
                        "BGN\n"
                        "\tINC A\n"
                        "\tDEC A\n"
                        "\tLD OUTPUT_1, A\n"
                        "END";

    LexicalAnalyzer lexer;
    FuncSyntacticAnalyzer parser;

    lexer.Tokenize(func);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);

    EXPECT_TRUE(parser.IsAccepted());

    auto funcRepresentation = dynamic_pointer_cast<FUNCIntermediateRepresentation>(intermediateRepresentation);
    EXPECT_NE(nullptr, funcRepresentation);

    EXPECT_STREQ("DOES_NOTHING", string(funcRepresentation->Identifier()).c_str());
    EXPECT_EQ(2llu, funcRepresentation->Line());
    EXPECT_EQ(1llu, funcRepresentation->Column());
    EXPECT_EQ(8llu, funcRepresentation->BodyTokens().size());
    EXPECT_EQ(0llu, funcRepresentation->InputArguments().size());
    EXPECT_EQ(0llu, funcRepresentation->OutputArguments().size());

    // Check body content
    EXPECT_EQ(TokenType::InstructionMnemonicINC, funcRepresentation->BodyTokens()[0].Type);
    EXPECT_STREQ(Lexemes::InstructionMnemonicINC.c_str(), funcRepresentation->BodyTokens()[0].Lexeme.c_str());
    EXPECT_EQ(4llu, funcRepresentation->BodyTokens()[0].Line);
    EXPECT_EQ(2llu, funcRepresentation->BodyTokens()[0].Column);
    
    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[1].Type);
    EXPECT_STREQ("A", funcRepresentation->BodyTokens()[1].Lexeme.c_str());
    EXPECT_EQ(4llu, funcRepresentation->BodyTokens()[1].Line);
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[1].Column);
    
    EXPECT_EQ(TokenType::InstructionMnemonicDEC, funcRepresentation->BodyTokens()[2].Type);
    EXPECT_STREQ(Lexemes::InstructionMnemonicDEC.c_str(), funcRepresentation->BodyTokens()[2].Lexeme.c_str());
    EXPECT_EQ(5llu, funcRepresentation->BodyTokens()[2].Line);
    EXPECT_EQ(2llu, funcRepresentation->BodyTokens()[2].Column);
    
    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[3].Type);
    EXPECT_STREQ("A", funcRepresentation->BodyTokens()[3].Lexeme.c_str());
    EXPECT_EQ(5llu, funcRepresentation->BodyTokens()[3].Line);
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[3].Column);
    
    EXPECT_EQ(TokenType::InstructionMnemonicLD, funcRepresentation->BodyTokens()[4].Type);
    EXPECT_STREQ(Lexemes::InstructionMnemonicLD.c_str(), funcRepresentation->BodyTokens()[4].Lexeme.c_str());
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[4].Line);
    EXPECT_EQ(2llu, funcRepresentation->BodyTokens()[4].Column);
    
    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[5].Type);
    EXPECT_STREQ("OUTPUT_1", funcRepresentation->BodyTokens()[5].Lexeme.c_str());
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[5].Line);
    EXPECT_EQ(5llu, funcRepresentation->BodyTokens()[5].Column);
    
    EXPECT_EQ(TokenType::SeparatorCOMMA, funcRepresentation->BodyTokens()[6].Type);
    EXPECT_STREQ(Lexemes::SeparatorCOMMA.c_str(), funcRepresentation->BodyTokens()[6].Lexeme.c_str());
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[6].Line);
    EXPECT_EQ(13llu, funcRepresentation->BodyTokens()[6].Column);

    EXPECT_EQ(TokenType::Identifier, funcRepresentation->BodyTokens()[7].Type);
    EXPECT_STREQ("A", funcRepresentation->BodyTokens()[7].Lexeme.c_str());
    EXPECT_EQ(6llu, funcRepresentation->BodyTokens()[7].Line);
    EXPECT_EQ(15llu, funcRepresentation->BodyTokens()[7].Column);
}