#pragma once 

#include <stdio.h>
#include <stdint.h>
#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class DMGObjectPallete0Register : public EightBitMemoryMappedRegisterBase
{
public:
    DMGObjectPallete0Register(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~DMGObjectPallete0Register() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
};

}