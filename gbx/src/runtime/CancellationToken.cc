#include "CancellationToken.h"

namespace gbx::runtime
{

void CancellationToken::Cancel()
{
    _isCancelled = true;
}

bool CancellationToken::IsCancelled()
{
    return _isCancelled;
}

}