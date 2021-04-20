#include "LCDWindowScrollXRegister.h"

using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDWindowScrollXRegister::LCDWindowScrollXRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}
     
void LCDWindowScrollXRegister::Write(variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->ScrollWindowX(_value);
}

}