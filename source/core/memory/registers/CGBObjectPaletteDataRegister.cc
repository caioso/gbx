#include "CGBObjectPaletteDataRegister.h"

#include "CGBObjectPaletteIndexRegister.h"

using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

CGBObjectPaletteDataRegister::CGBObjectPaletteDataRegister(gbxcore::interfaces::VideoControllerInterface* videoController)
    : _videoController(videoController)
{}

void CGBObjectPaletteDataRegister::Write(std::variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->RegisterCGBObjectPaletteColorByte(_currentColorIndex, _value);
    _indexRegisterReference->ReportDataWrite();
}

void CGBObjectPaletteDataRegister::RegisterIndexRegister(CGBObjectPaletteIndexRegister* reference)
{
    _indexRegisterReference = reference;
}

uint8_t CGBObjectPaletteDataRegister::CurrentColorIndex()
{
    return _currentColorIndex;
}

void CGBObjectPaletteDataRegister::SetCurrentColorIndex(uint8_t index)
{
    _currentColorIndex = index;
}

}