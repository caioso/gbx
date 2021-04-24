#include "CGBObjectPaletteIndexRegister.h"

#include "CGBObjectPaletteDataRegister.h"

using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

CGBObjectPaletteIndexRegister::CGBObjectPaletteIndexRegister(gbxcore::interfaces::VideoControllerInterface* videoController)
    : _videoController(videoController)
{}

void CGBObjectPaletteIndexRegister::Write(std::variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    
    if ((_value & 0x7F) >= 0x40)
        _value = (_value & 0x80) | 0x00;

    _dataRegisterReference->SetCurrentColorIndex(_value & 0x7F);
}

void CGBObjectPaletteIndexRegister::RegisterDataRegister(CGBObjectPaletteDataRegister* reference)
{
    _dataRegisterReference = reference;
}

void CGBObjectPaletteIndexRegister::ReportDataWrite()
{
    if ((_value & 0x80) != 0x00)
    {
        _value++;

        if ((_value & 0x7F) >= 0x40)
        _value = (_value & 0x80) | 0x00;

        _dataRegisterReference->SetCurrentColorIndex(_value & 0x7F);
    }
}

bool CGBObjectPaletteIndexRegister::AutoIncrementEnabled()
{
    if ((_value & 0x80) != 0x00)
        return true;
    return false;
}

}