#pragma once

namespace gbx
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