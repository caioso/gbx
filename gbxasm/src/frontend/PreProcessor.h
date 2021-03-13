#pragma once

#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../GBXAsmExceptions.h"
#include "../interfaces/Pass.h"

namespace gbxasm::frontend
{

class PreProcessor
{
public:
    PreProcessor() = default;
    ~PreProcessor() = default;

    PreProcessor(const PreProcessor&) = default;
    PreProcessor(PreProcessor&&) = default;
    PreProcessor& operator=(const PreProcessor&) = default;
    PreProcessor& operator=(PreProcessor&&) = default;

    void RegisterPass(std::shared_ptr<interfaces::Pass>, size_t);
    std::string ProcessPass(std::string, size_t);

protected:
    std::vector<std::shared_ptr<interfaces::Pass>> _passes;
};

}