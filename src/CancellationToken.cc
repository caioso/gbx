#include "CancellationToken.h"

namespace gbx
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