#pragma once

#include <stdlib.h>
#include <string> 

namespace gbxasm
{
struct Token
{
    std::string Token;
    std::size_t Line;
};

Token MakeToken(std::string, std::size_t);

}