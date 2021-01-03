#include "OpcodePatternMatcher.h"

namespace gbx
{

bool OpcodePatternMatcher::Match(uint8_t opcode, OpcodePattern opcodePattern)
{
    return ((opcode & opcodePattern.mask) ^ opcodePattern.pattern) == 0;
}

}