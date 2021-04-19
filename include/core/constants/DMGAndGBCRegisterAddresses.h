#pragma once

#include <stdlib.h>

namespace gbxcore::constants
{
    // LCD Register
    const size_t LCDControlRegisterAddress        = 0xFF40llu;
    const size_t LCDStatusAddress                 = 0xFF41llu;
    const size_t LCDScrollYAddress                = 0xFF42llu;
    const size_t LCDScrollXAddress                = 0xFF43llu;
    const size_t LCDScanLineYAddress              = 0xFF44llu;
    const size_t LCDScanLineYCompareAddress       = 0xFF45llu;
}