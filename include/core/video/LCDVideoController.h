#pragma once 

#include "ColorConverter.h"
#include "VideoControllerInterface.h"
#include "VideoOutputInterface.h"

namespace gbxcore::video
{

class LCDVideoController : public gbxcore::interfaces::VideoControllerInterface
{
public:
    LCDVideoController(gbxcore::interfaces::VideoOutputInterface*);
    virtual ~LCDVideoController() = default;

    bool IsVideoEnabled();
    bool IsWindowVisible();
    bool IsBackgroundVisible();
    bool AreSpritesVisible();

    void EnableVideo() override;
    void DisableVideo() override;
    void ShowWindow() override;
    void HideWindow() override;
    void ShowBackground() override;
    void HideBackground() override;
    void ShowSprites() override;
    void HideSprites() override;
    void SelectWindowTileMap(size_t) override;
    void SelectBackgroundTileMap(size_t) override;
    void SelectWindowAndBackgroundTileSet(size_t) override;
    void SetSpriteMode(uint8_t) override;
    void SetInterruptMode(uint8_t) override;
    void ScrollBackgroundX(size_t) override;
    void ScrollBackgroundY(size_t) override;
    void ScrollWindowX(size_t) override;
    void ScrollWindowY(size_t) override;
    void RegisterDMGBackgroundPaletteColor(uint8_t, gbxcore::interfaces::PaletteColor) override;
    void RegisterDMGObjectPaletteColor(uint8_t, gbxcore::interfaces::DMGPalette, gbxcore::interfaces::PaletteColor) override;
    void RegisterCGBBackgroundPaletteColorByte(uint8_t, uint8_t) override;
    void RegisterCGBObjectPaletteColorByte(uint8_t, uint8_t) override;

private:
    bool _isVideoEnabled{};
    bool _isWindowVisible{};
    bool _isBackgroundVisible{};
    bool _areSpritesVisible{};

    size_t _backgroundScrollX{};
    size_t _backgroundScrollY{};
    size_t _windowScrollX{};
    size_t _windowScrollY{};
    gbxcore::interfaces::VideoOutputInterface* _output;

    gbxcore::interfaces::OutputPalette _dmgOutputBackgroundPalette{};
    gbxcore::interfaces::OutputPalette _dmgOutputSpritePalette0{};
    gbxcore::interfaces::OutputPalette _dmgOutputSpritePalette1{};
};

}