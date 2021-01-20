#include "ParsedNumber.h"

using namespace std;

namespace gbxasm::utils
{

ParsedNumber::ParsedNumber(vector<Token>::iterator tokenIterator)
    : _tokenIterator(tokenIterator)
{}

void ParsedNumber::TryParse(Token token)
{
    ValidateToken(token.TokenWithoutDelimiter);
    EvaluatePrefixes(token.TokenWithoutDelimiter);
    EvaluateSuffixes(token.TokenWithoutDelimiter);
    
    auto suffixlessToken = RemoveSuffix(token.TokenWithoutDelimiter);
    ValidateNumericBase(suffixlessToken);
    ExtractNumericValue(suffixlessToken);
    EvaluateNumberType();
}

uint32_t ParsedNumber::Value()
{
    return _value;
}

uint32_t ParsedNumber::ModifiedValue()
{
    return ApplyModifier();
}

NumericBase ParsedNumber::Base()
{
    return _base;
}

NumericType ParsedNumber::Type()
{
    return _type;
}

ModifierType ParsedNumber::Modifier()
{
    return _modifier;
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
            c != 'c' && c != 'x' && c != 'X' && c != '\'' &&
            c != 'i' && c != 'I' && c != 'w' && c != 'W' && 
            c != 'g' && c != 'G' && c != 't' && c != 'T' &&
            c != ':')
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

void ParsedNumber::EvaluateSuffixes(string token)
{
    auto prefixlessNumber = RemovePrefix(token);
    auto suffix = ExtractSuffix(token);    

    if (suffix.size() == 0)
        _modifier = ModifierType::NoModifier;
    else
        ParseModifier(suffix, token);
}

inline void ParsedNumber::ParseModifier(string suffix, string token)
{
    if (regex highRegex(higherHalfRegex);
        regex_match(suffix, highRegex))
        _modifier = ModifierType::HigherHalf;
    else if (regex lowRegex(lowerHalfRegex);
             regex_match(suffix, lowRegex))
        _modifier = ModifierType::LowerHalf;
    else if (regex bitRegex(bitIndexRegex);
             regex_match(suffix, bitRegex))
    {
        ParseBitSuffix(token);
        _modifier = ModifierType::Bit;
    }
}

inline void ParsedNumber::ParseBitSuffix(string token)
{
    auto workignToken = token;
    if (workignToken.find_first_of('[') == string::npos)
    {
        auto arrayStartToken = (*(++_tokenIterator)).Token;
        if (arrayStartToken.compare("[") != 0)
        {
            stringstream ss;
            ss << "invalid '" << arrayStartToken << "' found near '" << token << "'";
            throw ParsedNumberException(ss.str());
        }

        workignToken += arrayStartToken;
    }

    // Accumulate strings up to the point the current index is closed
    if (workignToken.find_last_of(']') == string::npos)
    {
            
    }

    auto indexToken = workignToken.substr(workignToken.find_first_of('[') + 1, workignToken.find_last_of(']') - (workignToken.find_first_of('[') + 1));
    ParsedNumber indexNumber(_tokenIterator);
    indexNumber.TryParse(TokenMaker::MakeToken(indexToken, 0, 0));
    _bitSiffixIndex = indexNumber.ModifiedValue();    
}

string ParsedNumber::AcquireNextToken()
{
    return "";
}

inline string ParsedNumber::ExtractSuffix(string token)
{
    auto suffixPosition = token.find(':');

    if (suffixPosition == string::npos)
        return "";
    else 
        return token.substr(suffixPosition, token.size() - suffixPosition);
}

inline string ParsedNumber::RemoveSuffix(string token)
{
    auto suffixPosition = token.find(':');
    if (suffixPosition == string::npos)
        return token;
    else
        return token.substr(0, suffixPosition);
}

void ParsedNumber::ValidateNumericBase(string token)
{
    switch (_base)
    {
        case NumericBase::Hexadecimal: ValidateHexadecimal(token); break;
        case NumericBase::Decimal: ValidateDecimal(token); break;
        case NumericBase::Octal: ValidateOctal(token); break;
        case NumericBase::Binary: ValidateBinary(token); break;
    }
}

void ParsedNumber::ExtractNumericValue(string token)
{
    switch (_base)
    {
        case NumericBase::Hexadecimal: _value = ExtractValueFromHexadecimal(token); break;
        case NumericBase::Octal: _value = ExtractValueFromOctal(token); break;
        case NumericBase::Decimal: _value =ExtractValueFromDecimal(token); break;
        case NumericBase::Binary : _value = ExtractValueFromBinary(token); break;
    }
}

void ParsedNumber::EvaluateNumberType()
{
    if (_value <= 0xFF)
        _type = NumericType::Type8Bit;
    else if (_value > 0xFF && _value <= 0xFFFF)
        _type = NumericType::Type16Bit;
    else
        _type = NumericType::Type32Bit;
}

uint32_t ParsedNumber::ExtractValueFromHexadecimal(string token)
{
    auto prefixlessNumber = token.substr(2, token.size() - 2);
    auto counter = prefixlessNumber.size() - 1;
    auto result = 0;

    for (const auto &nibble : prefixlessNumber)
        result |= CharToNumber(nibble) << (counter--)*4;

    return result;
}

uint32_t ParsedNumber::ExtractValueFromOctal(string token)
{
    auto prefixlessNumber = token.substr(2, token.size() - 2);
    auto counter = prefixlessNumber.size() - 1;
    auto result = 0;

    for (const auto &digit : prefixlessNumber)
        result |= CharToNumber(digit) << (counter--)*3;

    return result;
}

uint32_t ParsedNumber::ExtractValueFromBinary(string token)
{
    auto prefixlessNumber = token.substr(2, token.size() - 2);
    auto clearNumber = RemoveDecimalPoint(prefixlessNumber);
    auto counter = clearNumber.size() - 1;;
    auto result = 0;

    for (const auto &digit : clearNumber)
        result |= CharToNumber(digit) << counter--;

    return result;
}

inline string ParsedNumber::RemoveDecimalPoint(string token)
{
    token.erase(std::remove(token.begin(), end(token), '.'), token.end());
    return token;
}

uint32_t ParsedNumber::ExtractValueFromDecimal(string token)
{
    auto prefixlessNumber = GetDecimalPrefixlessNumber(token);
    auto counter = (prefixlessNumber.size() - 1);
    auto result = 0;

    for (const auto &digit : prefixlessNumber)
        result += CharToNumber(digit)*(pow(10, counter--));

    return result;
}

inline string ParsedNumber::GetDecimalPrefixlessNumber(string token)
{
    auto prefix = token.substr(0, 2);
    return IsDecimalPrefix(prefix) ? token.substr(2, token.size() - 2) : token;
}

inline string ParsedNumber::RemovePrefix(string token)
{
    auto prefix = token.substr(0, 2);
    if (IsHexadecimalPrefix(prefix) || IsOctalPrefix(prefix) ||
        IsDecimalPrefix(prefix) || IsBinaryPrefix(prefix))
        return token.substr(2, token.size() - 2);
    return token;
}

inline uint8_t ParsedNumber::CharToNumber(char c)
{
    switch (c)
    {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'A': return 0xA;
        case 'a': return 0xa;
        case 'B': return 0xB;
        case 'b': return 0xb;
        case 'C': return 0xC;
        case 'c': return 0xc;
        case 'D': return 0xD;
        case 'd': return 0xd;
        case 'E': return 0xE;
        case 'e': return 0xe;
        case 'F': return 0xF;
        case 'f': return 0xf;
        default:
        {
            stringstream ss;
            ss << "Invalid character '" << c << "' found while parsing number";
            throw ParsedNumberException(ss.str());
        }
    }
}

inline void ParsedNumber::ValidateHexadecimal(std::string token)
{
    regex expression(hexadecimalRegex);
    if (!regex_match(token, expression))
    {
        stringstream ss;
        ss << "Invalid hexadecimal number '" << token << "'";
        throw ParsedNumberException(ss.str());
    }
}

inline void ParsedNumber::ValidateDecimal(std::string token)
{
    regex expression(decimalRegex);
    if (!regex_match(token, expression))
    {
        stringstream ss;
        ss << "Invalid decimal number '" << token << "'";
        throw ParsedNumberException(ss.str());
    }
}

inline void ParsedNumber::ValidateOctal(std::string token)
{
    regex expression(octalRegex);
    if (!regex_match(token, expression))
    {
        stringstream ss;
        ss << "Invalid octal number '" << token << "'";
        throw ParsedNumberException(ss.str());
    }
}

inline void ParsedNumber::ValidateBinary(std::string token)
{
    regex expression(binaryRegex);
    if (!regex_match(token, expression))
    {
        stringstream ss;
        ss << "Invalid binary number '" << token << "'";
        throw ParsedNumberException(ss.str());
    }
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

inline uint32_t ParsedNumber::ApplyModifier()
{
    switch (_modifier)
    {
        case ModifierType::HigherHalf: return ExtractHigherHalf();
        case ModifierType::LowerHalf: return ExtractLowerHalf();
        case ModifierType::Bit: return ExtractBitIndexValue();
        case ModifierType::NoModifier: return _value;
    }
}

inline uint32_t ParsedNumber::ExtractHigherHalf()
{
    if (_type == NumericType::Type8Bit)
        return (_value >> 0x04) & 0x0F;
    else if (_type == NumericType::Type16Bit)
        return (_value >> 0x08) & 0xFF;
    else
        return (_value >> 0x10) & 0xFFFF;
}

inline uint32_t ParsedNumber::ExtractLowerHalf()
{
    if (_type == NumericType::Type8Bit)
        return _value & 0x0F;
    else if (_type == NumericType::Type16Bit)
        return _value & 0xFF;
    else
        return _value & 0xFFFF;
}

inline uint32_t ParsedNumber::ExtractBitIndexValue()
{
    if (_type == NumericType::Type8Bit && _bitSiffixIndex >= 8)
    {   
        stringstream ss;
        ss << "invalid bit index '"<<_bitSiffixIndex<<"' for 8-bit number";
        throw ParsedNumberException(ss.str());
    }
    else if (_type == NumericType::Type16Bit && _bitSiffixIndex >= 16)
    {   
        stringstream ss;
        ss << "invalid bit index '"<<_bitSiffixIndex<<"' for 16-bit number";
        throw ParsedNumberException(ss.str());
    }
    else if (_type == NumericType::Type32Bit && _bitSiffixIndex >= 32)
    {   
        stringstream ss;
        ss << "invalid bit index '"<<_bitSiffixIndex<<"' for 32-bit number";
        throw ParsedNumberException(ss.str());
    }

    return (_value >> _bitSiffixIndex) & 0x01;
}

}