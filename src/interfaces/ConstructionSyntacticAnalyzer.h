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
    virtual frontend::AcceptedConstruction TryToAccept(std::vector<frontend::Token>::iterator&, std::vector<frontend::Token>::iterator&) = 0;

    bool IsAccepted();
    bool IsRejected();

protected:
    void Accept();
    void Reject();

    bool _isAccepted{};
    bool _isRejected{};
};

}