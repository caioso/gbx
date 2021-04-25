#pragma once

#include <stdlib.h>

namespace gbxcore::interfaces
{

enum class SpriteMode
{
    //  DMGBC Mode
    Sprite8x8,
    Sprite8x16
};

typedef struct OutputPalette_t
{
    uint8_t Color0;
    uint8_t Color1;
    uint8_t Color2;
    uint8_t Color3;
}
OutputPalette;

class VideoOutputInterface
{
public:
    virtual ~VideoOutputInterface() = default;

    virtual void SetVideoEnable(bool) = 0;
    virtual void SetWindowEnable(bool) = 0;
    virtual void SetBackgroundEnable(bool) = 0;
    virtual void SetSpriteEnable(bool) = 0;

    virtual void SetWindowTileMapBaseAddress(size_t) = 0;
    virtual void SetBackgroundTileMapBaseAddress(size_t) = 0;

    virtual void SetBackgroundAndWindowTileSetBaseAddress(size_t) = 0;

    virtual void SetSpriteMode(SpriteMode) = 0;

    virtual void SetWindowScrolXY(size_t, size_t) = 0;
    virtual void SetBackgroundScrolXY(size_t, size_t) = 0;

    virtual void SetDMGBackgroundPalette(OutputPalette) = 0;
    virtual void SetDMGSpritePalette(uint8_t, OutputPalette) = 0;
    
    virtual void SetCGBBackgroundPalette(uint8_t, OutputPalette) = 0;
    virtual void SetCGBSpritePalette(uint8_t, OutputPalette) = 0;
};

}