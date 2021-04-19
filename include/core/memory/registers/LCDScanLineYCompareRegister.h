#pragma once 

#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDScanLineYCompareRegister : public EightBitMemoryMappedRegisterBase
{
public:
    LCDScanLineYCompareRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDScanLineYCompareRegister() = default;

private:
    [[maybe_unused]] gbxcore::interfaces::VideoControllerInterface* _videoController;
};


}