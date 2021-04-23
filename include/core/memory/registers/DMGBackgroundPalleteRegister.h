#pragma once

#include <stdio.h>
#include <stdint.h>
#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class DMGBackgroundPalleteRegister : public EightBitMemoryMappedRegisterBase
{
public:
    DMGBackgroundPalleteRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~DMGBackgroundPalleteRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
};

}