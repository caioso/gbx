#pragma once

namespace gbx::runtime
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