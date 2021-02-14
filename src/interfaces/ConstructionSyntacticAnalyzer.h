#pragma once

#include <vector>

#include "../frontend/Token.h"
#include "../frontend/AcceptedConstruction.h"
#include "Construction.h"

namespace gbxasm::interfaces
{

class ConstructionSyntacticAnalyzer
{
public:
    virtual AcceptedConstruction TryToAccept(std::vector<Token>::iterator&, std::vector<Token>::iterator&) = 0;

    bool IsAccepted();
    bool IsRejected();

protected:
    void Accept();
    void Reject();

    bool _isAccepted{};
    bool _isRejected{};
};

}