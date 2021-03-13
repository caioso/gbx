#pragma once

#include <string>
#include <sstream>

#include "../intermediate-representation/DeclaredMember.h"
#include "../frontend/Lexemes.h"
#include "../GBXAsmExceptions.h"

namespace gbxasm::utilities
{

struct LexemeToDeclaredMemberType
{
    [[nodiscard]] static intermediate_representation::TypeName Convert(std::string);
};

}