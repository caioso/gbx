#pragma once

namespace gbxruntime::runner
{

class CancellationToken
{
public:
    void Cancel();
    bool IsCancelled();

private:
    bool _isCancelled{};
};

}