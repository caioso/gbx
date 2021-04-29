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
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
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

    virtual void Render() = 0;

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

    static RGBColor ByteToColor(uint8_t byte)
    {
        switch (byte)
        {
            case 0: return GBDLightest;
            case 2: return GBDSecondLightest;
            case 1: return GBDSecondDarkest;
            default: return GBDarkest;
        }
    }

private:
    inline static RGBColor GBDarkest = {15, 56, 15};
    inline static RGBColor GBDSecondDarkest = {48, 98, 48};
    inline static RGBColor GBDSecondLightest = {139, 172, 15};
    inline static RGBColor GBDLightest = {155, 188, 15};

};

}