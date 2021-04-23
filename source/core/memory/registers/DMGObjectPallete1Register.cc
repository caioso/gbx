#include "DMGObjectPallete1Register.h"

using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

DMGObjectPallete1Register::DMGObjectPallete1Register(gbxcore::interfaces::VideoControllerInterface* videoController)
    : _videoController(videoController)
{}

void DMGObjectPallete1Register::Write(std::variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    
    _videoController->RegisterDMGObjectPaletteColor(_value & 0x03, DMGPalette::Palette0, PaletteColor::Color0);
    _videoController->RegisterDMGObjectPaletteColor((_value >> 0x02) & 0x03, DMGPalette::Palette0, PaletteColor::Color1);
    _videoController->RegisterDMGObjectPaletteColor((_value >> 0x04) & 0x03, DMGPalette::Palette0, PaletteColor::Color2);
    _videoController->RegisterDMGObjectPaletteColor((_value >> 0x06) & 0x03, DMGPalette::Palette0, PaletteColor::Color3);
}

}