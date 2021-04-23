#include "CGBBackgroundPaletteDataRegister.h"

#include "CGBBackgroundPaletteIndexRegister.h"

using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

CGBBackgroundPaletteDataRegister::CGBBackgroundPaletteDataRegister(gbxcore::interfaces::VideoControllerInterface* videoController)
    : _videoController(videoController)
{}

void CGBBackgroundPaletteDataRegister::Write(std::variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->RegisterCGBBackgroundPaletteColorByte(_currentColorIndex, _value);
    _indexRegisterReference->ReportDataWrite();
}

void CGBBackgroundPaletteDataRegister::RegisterIndexRegister(CGBBackgroundPaletteIndexRegister* reference)
{
    _indexRegisterReference = reference;
}

uint8_t CGBBackgroundPaletteDataRegister::CurrentColorIndex()
{
    return _currentColorIndex;
}

void CGBBackgroundPaletteDataRegister::SetCurrentColorIndex(uint8_t index)
{
    _currentColorIndex = index;
}

}