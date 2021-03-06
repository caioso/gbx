#pragma once

#include <string>

namespace gbxasm::frontend
{

class Lexemes
{
public:
    // Keywords
    inline const static std::string KeywordPACK = "PACK";
    inline const static std::string KeywordFUNC = "FUNC";
    inline const static std::string KeywordEND = "END";
    inline const static std::string KeywordVAR = "VAR";
    inline const static std::string KeywordBOOL = "BOOL";
    inline const static std::string KeywordCHAR = "CHAR";
    inline const static std::string KeywordBYTE = "BYTE";
    inline const static std::string KeywordWORD = "WORD";
    inline const static std::string KeywordDWRD = "DWRD";
    inline const static std::string KeywordSTR = "STR";
    inline const static std::string KeywordAS = "AS";
    inline const static std::string KeywordCNST = "CNST";
    inline const static std::string KeywordFREE = "FREE";
    inline const static std::string KeywordIF = "IF";
    inline const static std::string KeywordTHEN = "THEN";
    inline const static std::string KeywordELSE = "ELSE";
    inline const static std::string KeywordWITH = "WITH";
    inline const static std::string KeywordREPT = "REPT";
    inline const static std::string KeywordNEXT = "NEXT";
    inline const static std::string KeywordEXIT = "EXIT";
    inline const static std::string KeywordWHEN = "WHEN";
    inline const static std::string KeywordIS = "IS";
    inline const static std::string KeywordFOR = "FOR";
    inline const static std::string KeywordUNTL = "UNTL";
    inline const static std::string KeywordTAG = "TAG";
    inline const static std::string KeywordTRY = "TRY";
    inline const static std::string KeywordEXPT = "EXPT";
    inline const static std::string KeywordTHRW = "THRW";
    inline const static std::string KeywordTEST = "TEST";
    inline const static std::string KeywordMOVE = "MOVE";
    inline const static std::string KeywordHIGH = "HIGH";
    inline const static std::string KeywordLOW = "LOW";
    inline const static std::string KeywordIN = "IN";
    inline const static std::string KeywordDFLT = "DFLT";
    inline const static std::string KeywordBRK = "BRK";
    inline const static std::string KeywordBGN = "BGN";
    inline const static std::string KeywordOUT = "OUT";
    inline const static std::string KeywordLOC = "LOC";
    inline const static std::string KeywordGBL = "GBL";

    // Operators
    inline const static std::string OperatorASSIGNMENT = "=";
    inline const static std::string OperatorEQUAL = "==";
    inline const static std::string OperatorPLUS = "+";
    inline const static std::string OperatorTRHEEWAYCOMPARISON = "<=>";
    inline const static std::string OperatorMINUS = "-";
    inline const static std::string OperatorMULTIPLICATION = "*";
    inline const static std::string OperatorDIVISION = "/";
    inline const static std::string OperatorBITWISEAND = "&";
    inline const static std::string OperatorBITWISEOR = "|";
    inline const static std::string OperatorBITWISENOT = "~";
    inline const static std::string OperatorBITWISEXOR = "^";
    inline const static std::string OperatorLEFTSHIFT = "<<";
    inline const static std::string OperatorRIGHTSHIFT = ">>";
    inline const static std::string OperatorDIFFERENT = "!=";
    inline const static std::string OperatorLOGICAND = "&&";
    inline const static std::string OperatorLOGICOR = "||";
    inline const static std::string OperatorLESSTHAN = "<";
    inline const static std::string OperatorGREATERTHAN = ">";
    inline const static std::string OperatorLESSTHANOREQUALTO = "<=";
    inline const static std::string OperatorGREATERTHANOREQUALTO = ">=";
    inline const static std::string OperatorLOGICNOT = "!";
    inline const static std::string OperatorAT = "@";
    inline const static std::string OperatorSEMICOLON = ":";
    inline const static std::string OperatorDOT = ".";
    inline const static std::string OperatorHASH = "#";
    inline const static std::string OperatorUNARYINCREMENT = "++";
    inline const static std::string OperatorUNARYDECREMENT = "--";

    // Separators
    inline const static std::string SeparatorCOMMA = ",";
    inline const static std::string SeparatorOPENPARENTHESIS = "(";
    inline const static std::string SeparatorCLOSEPARENTHESIS = ")";
    inline const static std::string SeparatorOPENCURLYBRACKETS = "{";
    inline const static std::string SeparatorCLOSECURLYBRACKETS = "}";
    inline const static std::string SeparatorOPENBRACKETS = "[";
    inline const static std::string SeparatorCLOSEBRACKETS = "]";

    // Literals
    inline const static std::string LiteralNumericHEXADECIMALBASE = "0x";
    inline const static std::string LiteralNumericDECIMALBASE = "0d";
    inline const static std::string LiteralNumericOCTALBASE = "0o";
    inline const static std::string LiteralNumericBINARYBASE = "0b";
    inline const static std::string LiteralBooleanTRUE = "TRUE";
    inline const static std::string LiteralBooleanFALSE = "FALSE";

    // Instructions Mnemonics
    inline const static std::string InstructionMnemonicNOP = "NOP";
    inline const static std::string InstructionMnemonicLD = "LD";
    inline const static std::string InstructionMnemonicINC = "INC";
    inline const static std::string InstructionMnemonicDEC = "DEC";
    inline const static std::string InstructionMnemonicRLCA = "RLCA";
    inline const static std::string InstructionMnemonicADD = "ADD";
    inline const static std::string InstructionMnemonicRRCA = "RRCA";
    inline const static std::string InstructionMnemonicSTOP = "STOP";
    inline const static std::string InstructionMnemonicRLA = "RLA";
    inline const static std::string InstructionMnemonicJR = "JR";
    inline const static std::string InstructionMnemonicRRA = "RRA";
    inline const static std::string InstructionMnemonicDAA = "DAA";
    inline const static std::string InstructionMnemonicCPL = "CPL";
    inline const static std::string InstructionMnemonicSCF = "SCF";
    inline const static std::string InstructionMnemonicCCF = "CCF";
    inline const static std::string InstructionMnemonicHALT = "HALT";
    inline const static std::string InstructionMnemonicADC = "ADC";
    inline const static std::string InstructionMnemonicSUB = "SUB";
    inline const static std::string InstructionMnemonicSBC = "SBC";
    inline const static std::string InstructionMnemonicAND = "AND";
    inline const static std::string InstructionMnemonicXOR = "XOR";
    inline const static std::string InstructionMnemonicOR = "OR";
    inline const static std::string InstructionMnemonicCP = "CP";
    inline const static std::string InstructionMnemonicRET = "RET";
    inline const static std::string InstructionMnemonicPOP = "POP";
    inline const static std::string InstructionMnemonicJP = "JP";
    inline const static std::string InstructionMnemonicPUSH = "PUSH";
    inline const static std::string InstructionMnemonicCALL = "CALL";
    inline const static std::string InstructionMnemonicRST = "RST";
    inline const static std::string InstructionMnemonicEI = "EI";
    inline const static std::string InstructionMnemonicDI = "DI";
    inline const static std::string InstructionMnemonicRLC = "RLC";
    inline const static std::string InstructionMnemonicRRC = "RRC";
    inline const static std::string InstructionMnemonicRL = "RL";
    inline const static std::string InstructionMnemonicRR = "RR";
    inline const static std::string InstructionMnemonicSLA = "SLA";
    inline const static std::string InstructionMnemonicSRA = "SRA";
    inline const static std::string InstructionMnemonicSWAP = "SWAP";
    inline const static std::string InstructionMnemonicSRL = "SRL";
    inline const static std::string InstructionMnemonicBIT = "BIT";
    inline const static std::string InstructionMnemonicRES = "RES";
    inline const static std::string InstructionMnemonicSET = "SET";
    inline const static std::string InstructionMnemonicJPU = "JPU";
    inline const static std::string InstructionMnemonicLDU = "LDU";

    // PreProcessor
    inline const static std::string PreProcessorIFDEF = ".IFDEF";
    inline const static std::string PreProcessorIFNDEF = ".IFNDEF";
    inline const static std::string PreProcessorBGN = ".BGN";
    inline const static std::string PreProcessorEND = ".END";
    inline const static std::string PreProcessorELSE = ".ELSE";
    inline const static std::string PreProcessorDEF = ".DEF";
    inline const static std::string PreProcessorUNDEF = ".UNDEF";
    
    inline const static std::string PreProcessorSingleLineCommentMarker = ";";
    inline const static std::string PreProcessorMultipleLineCommentMarkerStarter = ";:";
    inline const static std::string PreProcessorMultipleLineCommentMarkerEnder = ":;";
};

}