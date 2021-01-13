#include "Token.h"

using namespace std;

namespace gbxasm
{

Token MakeToken(std::string token, std::size_t line)
{
    return {.Token = token, .Line = line};
}

}