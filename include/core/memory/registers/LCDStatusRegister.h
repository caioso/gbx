#pragma once

#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDStatusRegister : public EightBitMemoryMappedRegisterBase
{
public:
    LCDStatusRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDStatusRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    inline void DecodeRegisterValue(uint8_t, uint8_t);

    gbxcore::interfaces::VideoControllerInterface* _videoController;
};


}