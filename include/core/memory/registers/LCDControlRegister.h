#pragma once

#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDControlRegister : public EightBitMemoryMappedRegisterBase
{
public:
    LCDControlRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDControlRegister() = default;
    
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    void ProcessValue(uint8_t);
    void HandleLCDControBit(uint8_t);
    void HandleWindowTileMapBit(uint8_t);
    void HandleWindowVisibilityBit(uint8_t);
    void HandleWindowAndBackgroundTileSetBit(uint8_t);
    void HandleBackgroundTileMapBit(uint8_t);
    void HandleSpriteModeBit(uint8_t);
    void HandleSpriteVisibilityBit(uint8_t);
    void HandleBackgroundAndWindowVisibilityBit(uint8_t, uint8_t);

    gbxcore::interfaces::VideoControllerInterface* _videoController;
};

}