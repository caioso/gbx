#include "LCDVideoController.h"

using namespace gbxcore::interfaces;
using namespace gbxcore::video;
using namespace std;

namespace gbxcore::video
{

LCDVideoController::LCDVideoController(VideoOutputInterface* output)
    : _output(output)
    
{}

bool LCDVideoController::IsVideoEnabled()
{
    return _isVideoEnabled;
}

bool LCDVideoController::IsWindowVisible()
{
    return _isWindowVisible;
}

bool LCDVideoController::IsBackgroundVisible()
{
    return _isBackgroundVisible;
}

bool LCDVideoController::AreSpritesVisible()
{
    return _areSpritesVisible;
}

void LCDVideoController::EnableVideo()
{
    _output->SetVideoEnable(true);
}

void LCDVideoController::DisableVideo()
{
    _output->SetVideoEnable(false);
}

void LCDVideoController::ShowWindow()
{
    _output->SetWindowEnable(true);
}

void LCDVideoController::HideWindow()
{
    _output->SetWindowEnable(false);
}

void LCDVideoController::ShowBackground()
{
    _output->SetBackgroundEnable(true);
}

void LCDVideoController::HideBackground()
{
    _output->SetBackgroundEnable(false);
}

void LCDVideoController::ShowSprites()
{
    _output->SetSpriteEnable(true);
}

void LCDVideoController::HideSprites()
{
    _output->SetSpriteEnable(false);
}

void LCDVideoController::SelectWindowTileMap(size_t addressBase)
{
    _output->SetWindowTileMapBaseAddress(addressBase);
}

void LCDVideoController::SelectBackgroundTileMap(size_t addressBase)
{
    _output->SetBackgroundTileMapBaseAddress(addressBase);
}

void LCDVideoController::SelectWindowAndBackgroundTileSet(size_t addressBase)
{
    _output->SetBackgroundAndWindowTileSetBaseAddress(addressBase);
}

void LCDVideoController::SetSpriteMode(uint8_t mode)
{
    if (mode == 0x00)
        _output->SetSpriteMode(gbxcore::interfaces::SpriteMode::Sprite8x8);
    else
        _output->SetSpriteMode(gbxcore::interfaces::SpriteMode::Sprite8x16);
}

void LCDVideoController::ScrollBackgroundX(size_t x)
{
    _backgroundScrollX = x;
    _output->SetBackgroundScrolXY(_backgroundScrollX, _backgroundScrollY);
}

void LCDVideoController::ScrollBackgroundY(size_t y)
{
    _backgroundScrollY = y;
    _output->SetBackgroundScrolXY(_backgroundScrollX, _backgroundScrollY);
}

void LCDVideoController::ScrollWindowX(size_t x)
{
    _windowScrollX = x;
    _output->SetWindowScrolXY(_windowScrollX, _windowScrollY);
}

void LCDVideoController::ScrollWindowY(size_t y)
{
    _windowScrollY = y;
    _output->SetWindowScrolXY(_windowScrollX, _windowScrollY);
}

void LCDVideoController::SetInterruptMode(uint8_t)
{}

void LCDVideoController::RegisterDMGBackgroundPaletteColor(uint8_t colorValue, PaletteColor colorIndex)
{
    if (colorIndex == PaletteColor::Color0)
        _dmgOutputBackgroundPalette.Color0 = ColorConverter::DMGColorToRGB(colorValue);
    else if (colorIndex == PaletteColor::Color1)
        _dmgOutputBackgroundPalette.Color1 = ColorConverter::DMGColorToRGB(colorValue);
    else if (colorIndex == PaletteColor::Color2)
        _dmgOutputBackgroundPalette.Color2 = ColorConverter::DMGColorToRGB(colorValue);
    else if (colorIndex == PaletteColor::Color3)
        _dmgOutputBackgroundPalette.Color3 = ColorConverter::DMGColorToRGB(colorValue);

    _output->SetDMGBackgroundPalette(_dmgOutputBackgroundPalette);
}

void LCDVideoController::RegisterDMGObjectPaletteColor(uint8_t colorValue, DMGPalette palette, PaletteColor colorIndex)
{
    auto dmgPalette = palette == DMGPalette::Palette0? _dmgOutputSpritePalette0 : _dmgOutputSpritePalette1;

    if (colorIndex == PaletteColor::Color0)
        dmgPalette.Color0 = ColorConverter::DMGColorToRGB(colorValue);
    else if (colorIndex == PaletteColor::Color1)
        dmgPalette.Color1 = ColorConverter::DMGColorToRGB(colorValue);
    else if (colorIndex == PaletteColor::Color2)
        dmgPalette.Color2 = ColorConverter::DMGColorToRGB(colorValue);
    else if (colorIndex == PaletteColor::Color3)
        dmgPalette.Color3 = ColorConverter::DMGColorToRGB(colorValue);

    if (palette == DMGPalette::Palette0)
        _output->SetDMGSpritePalette(0, dmgPalette);
    else
        _output->SetDMGSpritePalette(1, dmgPalette);
}

void LCDVideoController::RegisterCGBBackgroundPaletteColorByte(uint8_t, uint8_t)
{}

void LCDVideoController::RegisterCGBObjectPaletteColorByte(uint8_t, uint8_t)
{}


}