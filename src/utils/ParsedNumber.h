#pragma once

#include <cstdint>
#include <sstream>

#include "../GBXAsmExceptions.h"
#include "../Token.h"

namespace gbxasm::utils
{

enum class NumericBase
{
    Hexadecimal,
    Octal,
    Decimal,
    Binary
};

class ParsedNumber
{
public:
    ParsedNumber() = default;
    ~ParsedNumber() = default;

    void TryParse(Token);
    uint32_t Value();
    NumericBase Base();

private:
    void ValidateToken(std::string);
    void EvaluatePrefixes(std::string);

    inline bool IsHexadecimalPrefix(std::string);
    inline bool IsDecimalPrefix(std::string);
    inline bool IsOctalPrefix(std::string);
    inline bool IsBinaryPrefix(std::string);
    
    uint32_t _value;
    NumericBase _base;

    const std::string hexadecimalPrefixC = "0x";
    const std::string hexadecimalPrefixCCaptal = "0X";
    const std::string hexadecimalPrefixVerilog = "h'";
    const std::string hexadecimalPrefixVerilogCaptal = "H'";
    const std::string OctalPrefixC = "0o";
    const std::string OctalPrefixCCaptal = "0O";
    const std::string OctalPrefixVerilog = "o'";
    const std::string OctalPrefixVerilogCaptal = "O'";
    const std::string DecimalPrefixC = "0d";
    const std::string DecimalPrefixCCaptal = "0D";
    const std::string DecimalPrefixVerilog = "d'";
    const std::string DecimalPrefixVerilogCaptal = "D'";
    const std::string BinaryPrefixC = "0b";
    const std::string BinaryPrefixCCaptal = "0B";
    const std::string BinaryPrefixVerilog = "b'";
    const std::string BinaryPrefixVerilogCaptal = "B'";
};

}