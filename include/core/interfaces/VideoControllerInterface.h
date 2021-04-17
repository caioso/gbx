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
    virtual void ScrollBackgroundY(size_t) = 0;
    virtual void ScrollBackgroundX(size_t) = 0;
};

}