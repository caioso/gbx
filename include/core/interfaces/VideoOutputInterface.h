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

typedef struct RGBColor_t
{
    uint32_t Red;
    uint32_t Blue;
    uint32_t Green;
}
RGBColor;
typedef struct OutputPalette_t
{
    RGBColor Color0;
    RGBColor Color1;
    RGBColor Color2;
    RGBColor Color3;
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