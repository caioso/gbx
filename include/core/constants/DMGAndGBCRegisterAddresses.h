#pragma once

#include <stdlib.h>

namespace gbxcore::constants
{
    // LCD Register
    const size_t LCDControlRegisterAddress        = 0xFF40llu;
    const size_t LCDStatusAddress                 = 0xFF41llu;
    const size_t LCDBackgroundScrollYAddress      = 0xFF42llu;
    const size_t LCDBackgroundScrollXAddress      = 0xFF43llu;
    const size_t LCDScanLineYAddress              = 0xFF44llu;
    const size_t LCDScanLineYCompareAddress       = 0xFF45llu;
    const size_t LCDWindowScrollYAddress          = 0xFF4Allu;
    const size_t LCDWindowScrollXAddress          = 0xFF4Bllu;
}