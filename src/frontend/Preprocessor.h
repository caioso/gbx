#pragma once

#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../GBXAsmExceptions.h"
#include "../interfaces/Pass.h"

namespace gbxasm
{

class PreProcessor
{
public:
    PreProcessor() = default;
    ~PreProcessor() = default;

    void RegisterPass(std::shared_ptr<interfaces::Pass>, size_t);
    std::string ProcessPass(std::string, size_t);

protected:
    std::vector<std::shared_ptr<interfaces::Pass>> _passes;
};

}