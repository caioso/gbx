#pragma once 

#include <stdio.h>
#include <stdint.h>
#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class CGBBackgroundPaletteIndexRegister;

class CGBBackgroundPaletteDataRegister : public EightBitMemoryMappedRegisterBase
{
public:
    CGBBackgroundPaletteDataRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~CGBBackgroundPaletteDataRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;
    void RegisterIndexRegister(CGBBackgroundPaletteIndexRegister*);
    void SetCurrentColorIndex(uint8_t);
    uint8_t CurrentColorIndex();

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
    CGBBackgroundPaletteIndexRegister* _indexRegisterReference;
    uint8_t _currentColorIndex{};
};

}