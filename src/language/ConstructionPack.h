#pragma once

#include <vector>

#include "../interfaces/Construction.h"
#include "DeclaredMember.h"

namespace gbxasm::constructions
{

class ConstructionPack : public gbxasm::interfaces::Construction
{
public:
    ConstructionPack(size_t line, size_t column);
    virtual ~ConstructionPack() = default;

    std::string Identifier();
    std::vector<DeclaredMember> Members();

private:
    std::vector<DeclaredMember> _members;
};

}