#pragma once 

#include <stdio.h>
#include <stdint.h>
#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class CGBObjectPaletteIndexRegister;

class CGBObjectPaletteDataRegister : public EightBitMemoryMappedRegisterBase
{
public:
    CGBObjectPaletteDataRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~CGBObjectPaletteDataRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;
    void RegisterIndexRegister(CGBObjectPaletteIndexRegister*);
    void SetCurrentColorIndex(uint8_t);
    uint8_t CurrentColorIndex();

private:
    gbxcore::interfaces::VideoControllerInterface* _videoController;
    CGBObjectPaletteIndexRegister* _indexRegisterReference;
    uint8_t _currentColorIndex{};
};

}