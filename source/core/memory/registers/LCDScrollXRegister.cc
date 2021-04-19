#include "LCDScrollXRegister.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDScrollXRegister::LCDScrollXRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}

void LCDScrollXRegister::Write(variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->ScrollBackgroundX(_value);
}

}