#include "Token.h"

using namespace std;

namespace gbxasm
{

Token MakeToken(string token, size_t line, size_t column)
{
    return {.Token = token, .Line = line, .Column = column};
}

}