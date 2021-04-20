#pragma once 

#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDBackgroundScrollXRegister : public EightBitMemoryMappedRegisterBase
{
public:
    LCDBackgroundScrollXRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDBackgroundScrollXRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
};

}