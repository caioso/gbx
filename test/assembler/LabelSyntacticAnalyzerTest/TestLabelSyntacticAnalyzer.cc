#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "GBXAsmExceptions.h"
#include "LabelIntermediateRepresentation.h"
#include "LabelSyntacticAnalyzer.h"
#include "LexicalAnalyzer.h"
#include "Lexemes.h"

using namespace gbxasm;
using namespace gbxasm::frontend;
using namespace gbxasm::intermediate_representation;
using namespace gbxasm::frontend::parsers;
using namespace gbxasm::utilities;
using namespace std;

TEST(AssemblerTests_LabelSyntacticAnalysis, SanityCheckLabelParsing)
{
    const string expression = "MY_LABEL:";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorSEMICOLON}; // MY_LABEL:

    auto lexemes = { "MY_LABEL", Lexemes::OperatorSEMICOLON.c_str() }; // MY_LABEL:

    auto columns = { 1llu, 9llu}; // MY_LABEL:
    
    auto lines = { 1llu, 1llu}; // MY_LABEL:

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_LabelSyntacticAnalysis, SanityCheckLabelParsingGlobalLabel)
{
    const string expression = "MY_GLOBAL_LABEL<GBL>:";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorLESSTHAN, 
                        TokenType::KeywordGBL, TokenType::OperatorGREATERTHAN,
                        TokenType::OperatorSEMICOLON, }; // MY_GLOBAL_LABEL<GBL>:

    auto lexemes = { "MY_GLOBAL_LABEL", Lexemes::OperatorLESSTHAN.c_str(), 
                     Lexemes::KeywordGBL.c_str(), Lexemes::OperatorGREATERTHAN.c_str(),
                     Lexemes::OperatorSEMICOLON.c_str() }; // MY_GLOBAL_LABEL<GBL>:

    auto columns = { 1llu, 16llu, 17llu, 20llu, 21llu}; // MY_GLOBAL_LABEL<GBL>:
    
    auto lines = { 1llu, 1llu, 1llu, 1llu, 1llu}; // MY_GLOBAL_LABEL<GBL>:

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_LabelSyntacticAnalysis, SanityCheckLabelParsingLocalLabel)
{
    const string expression = "MY_LOCAL_LABEL<LOC>:";

    LexicalAnalyzer lexer;
    lexer.Tokenize(expression);
    auto tokens = lexer.Tokens();

    auto tokenTypes = { TokenType::Identifier, TokenType::OperatorLESSTHAN, 
                        TokenType::KeywordLOC, TokenType::OperatorGREATERTHAN,
                        TokenType::OperatorSEMICOLON, }; // MY_LOCAL_LABEL<LOC>:

    auto lexemes = { "MY_LOCAL_LABEL", Lexemes::OperatorLESSTHAN.c_str(), 
                     Lexemes::KeywordLOC.c_str(), Lexemes::OperatorGREATERTHAN.c_str(),
                     Lexemes::OperatorSEMICOLON.c_str() }; // MY_LOCAL_LABEL<LOC>:

    auto columns = { 1llu, 15llu, 16llu, 19llu, 20llu}; // MY_LOCAL_LABEL<LOC>:
    
    auto lines = { 1llu, 1llu, 1llu, 1llu, 1llu}; // MY_LOCAL_LABEL<LOC>:

    for (auto i = 0llu; i < lexemes.size(); ++i)
    {
        EXPECT_STREQ(*(begin(lexemes) + i), tokens[i].Lexeme.c_str());
        EXPECT_EQ(*(begin(tokenTypes) + i), tokens[i].Type);
        EXPECT_EQ(*(begin(lines) + i), tokens[i].Line);
        EXPECT_EQ(*(begin(columns) + i), tokens[i].Column);
    }
}

TEST(AssemblerTests_LabelSyntacticAnalysis, ParseLabel)
{
    const string expression = "MY_LABEL:";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsAccepted());
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ("MY_LABEL", expressionRepresentation->Identifier());
    EXPECT_EQ(LabelScope::Local, expressionRepresentation->Scope());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, ParseLabel2)
{
    const string expression = "MY_LABEL_WITH_EXTRAS:\n"
                              "\tLD A, 0x00";
    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsAccepted());
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ("MY_LABEL_WITH_EXTRAS", expressionRepresentation->Identifier());
    EXPECT_EQ(LabelScope::Local, expressionRepresentation->Scope());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, MalformedParseLabel)
{
    const string expression = "MY_LABEL HELLO:";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsRejected());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, MalformedParseLabel2)
{
    const string expression = "MY_LABEL";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsRejected());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, MalformedParseLabel3)
{
    const string expression = ":";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsRejected());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, MalformedParseLabel4)
{
    const string expression = ".Label:";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsRejected());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, MalformedParseLabel5)
{
    const string expression = "0x55:";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsRejected());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, MalformedParseLabel6)
{
    const string expression = "\"LABEL\":";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsRejected());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, ParseLabelWithScopeMarker)
{
    const string expression = "MY_LABEL<GBL>:";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    lexer.Tokenize(expression);
    
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsAccepted());
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ("MY_LABEL", expressionRepresentation->Identifier());
    EXPECT_EQ(LabelScope::Global, expressionRepresentation->Scope());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, ParseLabelWithScopeMarker2)
{
    const string expression = "MY_LABEL<LOC>:";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsAccepted());
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ("MY_LABEL", expressionRepresentation->Identifier());
    EXPECT_EQ(LabelScope::Local, expressionRepresentation->Scope());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, ParseLabelWithScopeMarker3)
{
    const string expression = "LABEL < LOC >    :";

    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsAccepted());
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ("LABEL", expressionRepresentation->Identifier());
    EXPECT_EQ(LabelScope::Local, expressionRepresentation->Scope());
}

TEST(AssemblerTests_LabelSyntacticAnalysis, ParseLabelWithScopeMarker4)
{
    const string expression = "MY_LABEL_WITH_EXTRAS<GBL>:\n"
                              "\tLD A, 0x00";
    LexicalAnalyzer lexer;
    LabelSyntacticAnalyzer parser;
    
    lexer.Tokenize(expression);
    auto currentToken = begin(lexer.Tokens());
    auto endIterator = end(lexer.Tokens());
    auto intermediateRepresentation = parser.TryToAccept(currentToken, endIterator);
    auto expressionRepresentation = dynamic_pointer_cast<LabelIntermediateRepresentation>(intermediateRepresentation);

    EXPECT_TRUE(parser.IsAccepted());
    EXPECT_NE(nullptr, expressionRepresentation);
    EXPECT_EQ("MY_LABEL_WITH_EXTRAS", expressionRepresentation->Identifier());
    EXPECT_EQ(LabelScope::Global, expressionRepresentation->Scope());
}
