#pragma once 

#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDWindowScrollXRegister : public EightBitMemoryMappedRegisterBase
{
public:
    LCDWindowScrollXRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDWindowScrollXRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
};


}