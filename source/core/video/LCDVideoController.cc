#include "LCDVideoController.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::video
{

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

LCDVideoController::LCDVideoController()
{}

void LCDVideoController::EnableVideo()
{}

void LCDVideoController::DisableVideo()
{}

void LCDVideoController::ShowWindow()
{}

void LCDVideoController::HideWindow()
{}

void LCDVideoController::ShowBackground()
{}

void LCDVideoController::HideBackground()
{}

void LCDVideoController::ShowSprites()
{}

void LCDVideoController::HideSprites()
{}

void LCDVideoController::SelectWindowTileMap(size_t)
{}

void LCDVideoController::SelectBackgroundTileMap(size_t)
{}

void LCDVideoController::SelectWindowAndBackgroundTileSet(size_t)
{}

void LCDVideoController::SetSpriteMode(uint8_t)
{}

void LCDVideoController::SetInterruptMode(uint8_t)
{}

void LCDVideoController::ScrollBackgroundX(size_t)
{}

void LCDVideoController::ScrollBackgroundY(size_t)
{}

void LCDVideoController::ScrollWindowX(size_t)
{}

void LCDVideoController::ScrollWindowY(size_t)
{}

void LCDVideoController::RegisterDMGBackgroundPaletteColor(uint8_t, PaletteColor)
{}

void LCDVideoController::RegisterDMGObjectPaletteColor(uint8_t, DMGPalette, PaletteColor)
{}

void LCDVideoController::RegisterCGBBackgroundPaletteColorByte(uint8_t, uint8_t)
{}

void LCDVideoController::RegisterCGBObjectPaletteColorByte(uint8_t, uint8_t)
{}


}