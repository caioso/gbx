#pragma once 

#include <stdio.h>
#include <stdint.h>
#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class DMGObjectPallete1Register : public EightBitMemoryMappedRegisterBase
{
public:
    DMGObjectPallete1Register(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~DMGObjectPallete1Register() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
};

}