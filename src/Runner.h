#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <variant>

#include "interfaces/Runtime.h"
#include "interfaces/DebugMessage.h"
#include "CancellationToken.h"

namespace gbx
{

class Runner
{
public:
    Runner(std::shared_ptr<gbxcore::interfaces::Runtime>);
    virtual ~Runner() = default;

    void Run(size_t, CancellationToken&);
    void Run(CancellationToken&);

    void RunWithDebugSupport(size_t, CancellationToken&);
    void RunWithDebugSupport(CancellationToken&);

    [[nodiscard]] std::variant<uint8_t, uint16_t> ReadRegister(gbxcore::interfaces::Register);
    
private:
    inline void RunHeadless();
    inline void RunWithDebugger();

private:
    std::shared_ptr<gbxcore::interfaces::Runtime> _runtime;
    std::queue<std::shared_ptr<interfaces::DebugMessage>> _requestQueue;
};

}