#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace gbxcore::interfaces
{

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
};

}