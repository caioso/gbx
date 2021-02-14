#pragma once

#include "../../interfaces/Pass.h"

namespace gbxasm
{

class ConditionalAssemblyPass : public interfaces::Pass
{ 
public:
    ConditionalAssemblyPass() = default;
    virtual ~ConditionalAssemblyPass() = default;

    virtual void Process(std::string) override;
    virtual std::string Result() override;
};

}