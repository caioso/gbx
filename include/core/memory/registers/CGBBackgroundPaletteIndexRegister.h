#pragma once 

#include <stdio.h>
#include <stdint.h>
#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class CGBBackgroundPaletteDataRegister;

class CGBBackgroundPaletteIndexRegister : public EightBitMemoryMappedRegisterBase
{
public:
    CGBBackgroundPaletteIndexRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~CGBBackgroundPaletteIndexRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;
    void RegisterDataRegister(CGBBackgroundPaletteDataRegister*);
    void ReportDataWrite();
    bool AutoIncrementEnabled();

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
    CGBBackgroundPaletteDataRegister* _dataRegisterReference;
    
};

}