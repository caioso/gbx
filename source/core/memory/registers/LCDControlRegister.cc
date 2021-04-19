#include "LCDControlRegister.h"

using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDControlRegister::LCDControlRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}

void LCDControlRegister::Write(variant<uint8_t, uint16_t> value)
{ 
    auto oldValue = _value;
    
    _value = Extract8BitValue(value);
    ProcessValue(oldValue);
}

void LCDControlRegister::ProcessValue(uint8_t oldValue)
{
    // Check Bit 7 (LDC Enable/Disable)
    if ((oldValue & 0x80) != (_value & 0x80))
        HandleLCDControBit(_value >> 0x07);
    // Check Bit 6 (Window Tile Map)
    if ((oldValue & 0x40) != (_value & 0x40))
        HandleWindowTileMapBit((_value >> 0x06) & 0x01);
    // Check Bit 5 (Window Visibility)
    if ((oldValue & 0x20) != (_value & 0x20))
        HandleWindowVisibilityBit((_value >> 0x05) & 0x01);
    // Check Bit 4 (Window & Background Tile Set)
    if ((oldValue & 0x10) != (_value & 0x10))
        HandleWindowAndBackgroundTileSetBit((_value >> 0x04) & 0x01);
    // Check Bit 3 (Background Tile Map)
    if ((oldValue & 0x08) != (_value & 0x08))
        HandleBackgroundTileMapBit((_value >> 0x03) & 0x01);
    // Check Bit 2 (Sprite mode)
    if ((oldValue & 0x04) != (_value & 0x04))
        HandleSpriteModeBit((_value >> 0x02) & 0x01);
    // Check Bit 1 (Sprite Visibility)
    if ((oldValue & 0x02) != (_value & 0x02))
        HandleSpriteVisibilityBit((_value >> 0x01) & 0x01);
    // Check Bit 0 (Window and Background Visibility)
    if ((oldValue & 0x01) != (_value & 0x01))
        HandleBackgroundAndWindowVisibilityBit(_value & 0x01, (_value >> 0x05) & 0x01);
}

void LCDControlRegister::HandleLCDControBit(uint8_t bit)
{
    if (bit == 0x01)
        _videoController->EnableVideo();
    else
        _videoController->DisableVideo();

}

void LCDControlRegister::HandleWindowTileMapBit(uint8_t bit)
{
    if (bit == 0x01)
        _videoController->SelectWindowTileMap(0x9C00llu);
    else
        _videoController->SelectWindowTileMap(0x9800llu);

}

void LCDControlRegister::HandleWindowVisibilityBit(uint8_t bit)
{
    if (bit == 0x01)
        _videoController->ShowWindow();
    else
        _videoController->HideWindow();
}

void LCDControlRegister::HandleWindowAndBackgroundTileSetBit(uint8_t bit)
{
    if (bit == 0x01)
        _videoController->SelectWindowAndBackgroundTileSet(0x8000llu);
    else
        _videoController->SelectWindowAndBackgroundTileSet(0x8800llu);
}

void LCDControlRegister::HandleBackgroundTileMapBit(uint8_t bit)
{
    if (bit == 0x01)
        _videoController->SelectBackgroundTileMap(0x9C00llu);
    else
        _videoController->SelectBackgroundTileMap(0x9800llu);
}

void LCDControlRegister::HandleSpriteModeBit(uint8_t bit)
{
    _videoController->SetSpriteMode(bit);
}

void LCDControlRegister::HandleSpriteVisibilityBit(uint8_t bit)
{
    if (bit == 0x01)
        _videoController->ShowSprites();
    else
        _videoController->HideSprites();
}

void LCDControlRegister::HandleBackgroundAndWindowVisibilityBit(uint8_t bit, uint8_t windowBit)
{
    if (bit == 0x01 && windowBit == 0x00)
        _videoController->ShowBackground();
    else if (bit == 0x01 && windowBit == 0x01)
    {
        _videoController->ShowBackground();
        _videoController->ShowWindow();
    }
    else if (bit == 0x00 && windowBit == 0x01)
    {
        _videoController->HideBackground();
        _videoController->HideWindow();
    }
    else if (bit == 0x00 && windowBit == 0x00)
        _videoController->HideBackground();
}

}