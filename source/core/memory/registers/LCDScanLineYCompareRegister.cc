#include "LCDScanLineYCompareRegister.h"

using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

LCDScanLineYCompareRegister::LCDScanLineYCompareRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}

}