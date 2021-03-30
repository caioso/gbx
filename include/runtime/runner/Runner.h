#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <variant>
#include <thread>

#include "Runtime.h"
#include "CancellationToken.h"

namespace gbxruntime::runner
{

enum class RunnerMode
{
    Runtime,
    Debug
};

class Runner : public std::enable_shared_from_this<Runner>
{
public:
    Runner(std::shared_ptr<gbxcore::interfaces::Runtime>);
    virtual ~Runner() = default;

    void Run(CancellationToken&);
    void Run(size_t, CancellationToken&);

    [[nodiscard]] RunnerMode Mode();
    
private:
    inline void RunInDebugMode();
    inline void RunHeadless(CancellationToken&);
    inline void RunHeadless(size_t, CancellationToken&);
    inline void RunWithDebugger(CancellationToken&);
    inline void RunWithDebugger(size_t, CancellationToken&);
    inline void InitializeDebugInfraIfNeeded();

    std::shared_ptr<gbxcore::interfaces::Runtime> _runtime;    
    RunnerMode _mode;
};

}