#include "DMGBackgroundPalleteRegister.h"

using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

DMGBackgroundPalleteRegister::DMGBackgroundPalleteRegister(gbxcore::interfaces::VideoControllerInterface* videoController)
    : _videoController(videoController)
{}

void DMGBackgroundPalleteRegister::Write(std::variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    
    _videoController->RegisterDMGBackgroundPaletteColor(_value & 0x03, PaletteColor::Color0);
    _videoController->RegisterDMGBackgroundPaletteColor((_value >> 0x02) & 0x03, PaletteColor::Color1);
    _videoController->RegisterDMGBackgroundPaletteColor((_value >> 0x04) & 0x03, PaletteColor::Color2);
    _videoController->RegisterDMGBackgroundPaletteColor((_value >> 0x06) & 0x03, PaletteColor::Color3);
}

}