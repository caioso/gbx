#pragma once 

#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDScrollYRegister : public EightBitMemoryMappedRegisterBase
{
public:
    LCDScrollYRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDScrollYRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
};


}