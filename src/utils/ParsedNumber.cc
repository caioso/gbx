#include "ParsedNumber.h"

using namespace std;

namespace gbxasm::utils
{

void ParsedNumber::TryParse(Token token)
{
    ValidateToken(token.TokenWithoutDelimiter);
    EvaluatePrefixes(token.TokenWithoutDelimiter);

    _value = 1234;
}

uint32_t ParsedNumber::Value()
{
    return _value;
}

NumericBase ParsedNumber::Base()
{
    return _base;
}

void ParsedNumber::ValidateToken(string token)
{
    for (auto c : token)
    {
        if (c != '0' && c != '1' && c != '2' && c != '3' &&
            c != '4' && c != '5' && c != '6' && c != '7' &&
            c != '8' && c != '9' && c != 'A' && c != 'a' &&
            c != 'B' && c != 'b' && c != 'C' && c != 'c' && 
            c != 'D' && c != 'd' && c != 'E' && c != 'e' &&
            c != 'F' && c != 'f' && c != '.' && c != 'L' &&
            c != 'l' && c != 'L' && c != '[' && c != ']' &&
            c != 'o' && c != 'O' && c != 'h' && c != 'H' &&
            c != 'c' && c != 'x' && c != 'X' && c != '\'' )
        {
            stringstream ss;
            ss << "Invalid character '" << c << "' in numeric constant '" << token << "'";
            throw ParsedNumberException(ss.str());
        }
    }
}

void ParsedNumber::EvaluatePrefixes(string token)
{
    if (auto prefix = token.substr(0, 2);
        IsHexadecimalPrefix(prefix))
        _base = NumericBase::Hexadecimal;
    else if (IsDecimalPrefix(prefix))
        _base = NumericBase::Decimal;
    else if (IsOctalPrefix(prefix))
        _base = NumericBase::Octal;
    else if (IsBinaryPrefix(prefix))
             _base = NumericBase::Binary;
    else
        _base = NumericBase::Decimal;
}

inline bool ParsedNumber::IsHexadecimalPrefix(string prefix)
{
    return prefix.compare(hexadecimalPrefixC) == 0 ||
           prefix.compare(hexadecimalPrefixCCaptal) == 0 ||
           prefix.compare(hexadecimalPrefixVerilog) == 0 ||
           prefix.compare(hexadecimalPrefixVerilogCaptal) == 0;
}

inline bool ParsedNumber::IsDecimalPrefix(string prefix)
{
    return prefix.compare(DecimalPrefixC) == 0 ||
           prefix.compare(DecimalPrefixCCaptal) == 0 ||
           prefix.compare(DecimalPrefixVerilog) == 0 ||
           prefix.compare(DecimalPrefixVerilogCaptal) == 0;
}

inline bool ParsedNumber::IsOctalPrefix(string prefix)
{
    return prefix.compare(OctalPrefixC) == 0 ||
           prefix.compare(OctalPrefixCCaptal) == 0 ||
           prefix.compare(OctalPrefixVerilog) == 0 ||
           prefix.compare(OctalPrefixVerilogCaptal) == 0;
}

inline bool ParsedNumber::IsBinaryPrefix(string prefix)
{
    return prefix.compare(BinaryPrefixC) == 0 ||
           prefix.compare(BinaryPrefixCCaptal) == 0 ||
           prefix.compare(BinaryPrefixVerilog) == 0 ||
           prefix.compare(BinaryPrefixVerilogCaptal) == 0;
}

}