#include "CGBBackgroundPaletteIndexRegister.h"

#include "CGBBackgroundPaletteDataRegister.h"

using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

CGBBackgroundPaletteIndexRegister::CGBBackgroundPaletteIndexRegister(gbxcore::interfaces::VideoControllerInterface* videoController)
    : _videoController(videoController)
{}

void CGBBackgroundPaletteIndexRegister::Write(std::variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    
    if ((_value & 0x7F) >= 0x40)
        _value = (_value & 0x80) | 0x00;

    _dataRegisterReference->SetCurrentColorIndex(_value & 0x7F);
}

void CGBBackgroundPaletteIndexRegister::ReportDataWrite()
{
    if ((_value & 0x80) != 0x00)
    {
        _value++;

        if ((_value & 0x7F) >= 0x40)
        _value = (_value & 0x80) | 0x00;

        _dataRegisterReference->SetCurrentColorIndex(_value & 0x7F);
    }
}

void CGBBackgroundPaletteIndexRegister::RegisterDataRegister(CGBBackgroundPaletteDataRegister* reference)
{
    _dataRegisterReference = reference;
}

bool CGBBackgroundPaletteIndexRegister::AutoIncrementEnabled()
{
    if ((_value & 0x80) != 0x00)
        return true;
    return false;
}

}