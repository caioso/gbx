#pragma once

#include <cstdlib>
#include <memory>
#include <sstream>
#include <vector>

#include "../GBXAsmExceptions.h"
#include "../interfaces/Pass.h"

namespace gbxasm
{

class Preprocessor
{
public:
    Preprocessor() = default;
    ~Preprocessor() = default;

    void RegisterPass(std::shared_ptr<interfaces::Pass>, size_t);
    void ProcessPass(size_t);

protected:
    std::vector<std::shared_ptr<interfaces::Pass>> _passes;
};

}