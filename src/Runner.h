#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <variant>

#include "interfaces/Runtime.h"
#include "interfaces/DebugRequestConsumer.h"
#include "interfaces/DebugRequestProducer.h"
#include "CancellationToken.h"

namespace gbx
{

class Runner : public interfaces::DebugRequestConsumer
{
public:
    Runner(std::shared_ptr<gbxcore::interfaces::Runtime>, std::shared_ptr<interfaces::DebugRequestProducer>);
    virtual ~Runner() = default;

    void Run(size_t, CancellationToken&);
    void Run(CancellationToken&);

    void RunWithDebugSupport(size_t, CancellationToken&);
    void RunWithDebugSupport(CancellationToken&);

    [[nodiscard]] std::variant<uint8_t, uint16_t> ReadRegister(gbxcore::interfaces::Register);
    
    virtual void ConsumeRequest(std::shared_ptr<interfaces::DebugRequest>) override;

private:
    inline void RunHeadless();
    inline void RunWithDebugger();

private:
    std::shared_ptr<gbxcore::interfaces::Runtime> _runtime;
    std::queue<std::shared_ptr<interfaces::DebugRequest>> _requestQueue;
};

}