#pragma once

#include <cstdint>

namespace gbx
{

typedef enum class BitPattern_t
{
    _0,
    _1,
    _X,
}
b;

typedef struct OpcodePattern_t
{
    uint8_t mask;
    uint8_t pattern;
}
OpcodePattern;

class OpcodePatternMatcher
{
public:
    inline static bool Match(uint8_t opcode, OpcodePattern opcodePattern)
    {
        return ((opcode & opcodePattern.mask) ^ opcodePattern.pattern) == 0;
    }

    constexpr static OpcodePattern Pattern(BitPattern_t p7, BitPattern_t p6, BitPattern_t p5, BitPattern_t p4, BitPattern_t p3, BitPattern_t p2, BitPattern_t p1, BitPattern_t p0)
    {
        return {
                static_cast<uint8_t>(
                ((p7 == b::_X)? 0 : 1) << 7 | 
                ((p6 == b::_X)? 0 : 1) << 6 |
                ((p5 == b::_X)? 0 : 1) << 5 |
                ((p4 == b::_X)? 0 : 1) << 4 |
                ((p3 == b::_X)? 0 : 1) << 3 |
                ((p2 == b::_X)? 0 : 1) << 2 |
                ((p1 == b::_X)? 0 : 1) << 1 |
                ((p0 == b::_X)? 0 : 1)),
                static_cast<uint8_t>(
                ((p7 == b::_0 || p7 == b::_1)? static_cast<uint8_t>(p7) : 0) << 7 | 
                ((p6 == b::_0 || p6 == b::_1)? static_cast<uint8_t>(p6) : 0) << 6 |
                ((p5 == b::_0 || p5 == b::_1)? static_cast<uint8_t>(p5) : 0) << 5 |
                ((p4 == b::_0 || p4 == b::_1)? static_cast<uint8_t>(p4) : 0) << 4 |
                ((p3 == b::_0 || p3 == b::_1)? static_cast<uint8_t>(p3) : 0) << 3 |
                ((p2 == b::_0 || p2 == b::_1)? static_cast<uint8_t>(p2) : 0) << 2 |
                ((p1 == b::_0 || p1 == b::_1)? static_cast<uint8_t>(p1) : 0) << 1 |
                ((p0 == b::_0 || p0 == b::_1)? static_cast<uint8_t>(p0) : 0))
                };
    }
};

}