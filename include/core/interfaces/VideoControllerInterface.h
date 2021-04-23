#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace gbxcore::interfaces
{

enum class VideoInterruptSource : uint8_t
{
    HBlankInterrupt = 1,
    VBlankInterrupt = 2,
    OAMSearchInterrupt = 4,
    LCDDataTransferInterrupt = 8
};

enum class PaletteColor : uint8_t
{
    Color0,
    Color1,
    Color2,
    Color3
};

enum class DMGPalette : uint8_t
{
    Palette0,
    Palette1,
};

class VideoControllerInterface
{
public:
    virtual ~VideoControllerInterface() = default;

    virtual void EnableVideo() = 0;
    virtual void DisableVideo() = 0;
    virtual void ShowWindow() = 0;
    virtual void HideWindow() = 0;
    virtual void ShowBackground() = 0;
    virtual void HideBackground() = 0;
    virtual void ShowSprites() = 0;
    virtual void HideSprites() = 0;
    virtual void SelectWindowTileMap(size_t) = 0;
    virtual void SelectBackgroundTileMap(size_t) = 0;
    virtual void SelectWindowAndBackgroundTileSet(size_t) = 0;
    virtual void SetSpriteMode(uint8_t) = 0;
    virtual void SetInterruptMode(uint8_t) = 0;
    virtual void ScrollBackgroundX(size_t) = 0;
    virtual void ScrollBackgroundY(size_t) = 0;
    virtual void ScrollWindowX(size_t) = 0;
    virtual void ScrollWindowY(size_t) = 0;
    virtual void RegisterDMGBackgroundPaletteColor(uint8_t, PaletteColor) = 0;
    virtual void RegisterDMGObjectPaletteColor(uint8_t, DMGPalette, PaletteColor) = 0;
    virtual void RegisterCGBBackgroundPaletteColorByte(uint8_t, uint8_t) = 0;
};

}