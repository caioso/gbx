#pragma once

#include <algorithm>
#include <cstdint>
#include <math.h>
#include <regex>
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

enum class NumericType
{
    Unsiged8Bits,
    Signed8Bits,
    Unsigned16Bits,
    Signed16Bits,
    Unsigned32Bits,
    Signed32Bits,
};

class ParsedNumber
{
public:
    ParsedNumber() = default;
    ~ParsedNumber() = default;

    void TryParse(Token);
    uint32_t Value();
    NumericBase Base();
    NumericType Type();

private:
    void ValidateToken(std::string);
    void EvaluatePrefixes(std::string);
    void ValidateNumericBase(std::string);

    inline bool IsHexadecimalPrefix(std::string);
    inline bool IsDecimalPrefix(std::string);
    inline bool IsOctalPrefix(std::string);
    inline bool IsBinaryPrefix(std::string);

    inline void ValidateHexadecimal(std::string);
    inline void ValidateDecimal(std::string);
    inline void ValidateOctal(std::string);
    inline void ValidateBinary(std::string);

    inline uint32_t ExtractNumericValue(std::string);
    inline uint32_t ExtractValueFromHexadecimal(std::string);
    inline uint32_t ExtractValueFromOctal(std::string);
    inline uint32_t ExtractValueFromDecimal(std::string);
    inline uint32_t ExtractValueFromBinary(std::string);

    inline uint8_t CharToNumber(char);
    inline std::string GetDecimalPrefixlessNumber(std::string);
    inline std::string RemoveDecimalPoint(std::string);

    uint32_t _value;
    NumericBase _base;
    NumericType _type;

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

    const std::string hexadecimalRegex = "^((0x|0X|H\'|h\')(([0123456789ABCDF]|[0123456789abcdef])+))$";
    const std::string decimalRegex = "^((0d|0D|D\'|d\')?(([0123456789])+))$";
    const std::string octalRegex = "^((0o|0O|O\'|o\')?(([01234567])+))$";
    const std::string binaryRegex = "^((0b|0B|B\'|b\')?([01]+(\\.[01]+)*))$";
};

}