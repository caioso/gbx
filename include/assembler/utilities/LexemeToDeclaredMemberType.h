#pragma once

#include <string>
#include <sstream>

#include "DeclaredMember.h"
#include "Lexemes.h"
#include "GBXAsmExceptions.h"

namespace gbxasm::utilities
{

struct LexemeToDeclaredMemberType
{
    [[nodiscard]] static intermediate_representation::TypeName Convert(std::string);
};

}