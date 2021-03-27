#include "CancellationToken.h"

namespace gbxruntime::runner
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