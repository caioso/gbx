#pragma once 

#include <stdio.h>
#include <stdint.h>
#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class CGBObjectPaletteDataRegister;

class CGBObjectPaletteIndexRegister : public EightBitMemoryMappedRegisterBase
{
public:
    CGBObjectPaletteIndexRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~CGBObjectPaletteIndexRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;
    void RegisterDataRegister(CGBObjectPaletteDataRegister*);
    void ReportDataWrite();
    bool AutoIncrementEnabled();

private:
    [[maybe_unused]] gbxcore::interfaces::VideoControllerInterface* _videoController;
    CGBObjectPaletteDataRegister* _dataRegisterReference;   
};

}