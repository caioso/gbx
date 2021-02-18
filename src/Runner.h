#pragma once

#include <cstdint>
#include <memory>

#include "interfaces/Runtime.h"
#include "CancellationToken.h"

namespace gbx
{

class Runner
{
public:
    Runner(std::shared_ptr<interfaces::Runtime>);
    virtual ~Runner() = default;

    void Run();
    void Run(size_t numberOfCycles);
    void Run(CancellationToken&);

private:
    std::shared_ptr<interfaces::Runtime> _runtime;
};

}