#include "Token.h"

using namespace std;

namespace gbxasm
{

Token TokenMaker::MakeToken(string token, size_t line, size_t column)
{
    auto cleanString = RemoveDelimieters(token);
    return {.Token = token, .Line = line, .Column = column, .TokenWithoutDelimiter = cleanString};
}

string TokenMaker::RemoveDelimieters(string token)
{
    auto start = static_cast<size_t>(0);
    auto end = token.size() - 1;
    
    for(; start < token.size(); ++start)
        if (token[start] != ',' && token[start] != ';')
            break;
    
    for(; end >= static_cast<size_t>(0); --end)
        if (token[end] != ',' && token[end] != ';')
            break;

    return token.substr(start, end - start + 1);
}

}