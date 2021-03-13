#pragma once

#include <string>

namespace gbxasm::utilities
{

class IdentifierValidator
{

public:
    static bool IsValid(std::string_view);
    static bool IsInitialDigit(std::string_view, size_t);

};

}