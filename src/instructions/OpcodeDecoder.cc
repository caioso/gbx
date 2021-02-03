#include "OpcodeDecoder.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

shared_ptr<InstructionInterface> OpcodeDecoder::DecodeOpcode(uint8_t opcode, optional<uint8_t> preOpcode)
{
    if (preOpcode.has_value() && 
       (preOpcode.value() == InstructionConstants::PreOpcode_CB || 
        preOpcode.value() == InstructionConstants::PreOpcode_DD || 
        preOpcode.value() == InstructionConstants::PreOpcode_FD))
        return DecodeInstructionWithPreOpcode(opcode, preOpcode);
    else
        return DecodeInstructionWithoutPreOpcode(opcode);
}

shared_ptr<InstructionInterface> OpcodeDecoder::DecodeInstructionWithPreOpcode(uint8_t opcode, std::optional<uint8_t> preOpcode)
{
    if (preOpcode.value() == InstructionConstants::PreOpcode_CB)
    {
        if (OpcodePatternMatcher::Match(opcode, // 01XX XXXX
            OpcodePatternMatcher::Pattern('0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b)))
            return make_shared<InstructionBit>();
        else if (OpcodePatternMatcher::Match(opcode, // 10XX XXXX
                OpcodePatternMatcher::Pattern('1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionRes>();
        else if (OpcodePatternMatcher::Match(opcode, // 11XX XXXX
                OpcodePatternMatcher::Pattern('1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionSet>();
        else if (OpcodePatternMatcher::Match(opcode, // 0000 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionRlc>();
        else if (OpcodePatternMatcher::Match(opcode, // 0001 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionRl>();
        else if (OpcodePatternMatcher::Match(opcode, // 0000 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionRrc>();
        else if (OpcodePatternMatcher::Match(opcode, // 0001 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionRr>();
        else if (OpcodePatternMatcher::Match(opcode, // 0010 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionSla>();
        else if (OpcodePatternMatcher::Match(opcode, // 0010 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionSra>();
        else if (OpcodePatternMatcher::Match(opcode, // 0011 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionSrl>();
        else if (OpcodePatternMatcher::Match(opcode, // 0011 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return make_shared<InstructionSwap>();
    }
    else if (preOpcode.value() == InstructionConstants::PreOpcode_DD ||
             preOpcode.value() == InstructionConstants::PreOpcode_FD)
    {
        if (OpcodePatternMatcher::Match(opcode, // 01XX X110
            OpcodePatternMatcher::Pattern('0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '1'_b, '0'_b)) ||
            OpcodePatternMatcher::Match(opcode, // 0111 0XXX
            OpcodePatternMatcher::Pattern('0'_b, '1'_b, '1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
            return make_shared<InstructionLd>();
    }
    
    throw InstructionException("unknown instruction");
}

shared_ptr<InstructionInterface> OpcodeDecoder::DecodeInstructionWithoutPreOpcode(uint8_t opcode)
{
    if (OpcodePatternMatcher::Match(opcode, // 0000 0111
        OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b)))
        return make_shared<InstructionRlca>();
    else if (OpcodePatternMatcher::Match(opcode, // 0001 0111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b)))
             return make_shared<InstructionRla>();
    else if (OpcodePatternMatcher::Match(opcode, // 0001 1000
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 001X X000
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '0'_b)))
             return make_shared<InstructionJr>();
    else if (OpcodePatternMatcher::Match(opcode, // 1110 1001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '0'_b, '1'_b)) || 
             OpcodePatternMatcher::Match(opcode, // 110X X01X
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, '0'_b, '1'_b, 'X'_b)))
             return make_shared<InstructionJp>();
    else if (OpcodePatternMatcher::Match(opcode, // 0000 1111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b)))
             return make_shared<InstructionRrca>();
    else if (OpcodePatternMatcher::Match(opcode, // 0001 1111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b)))
             return make_shared<InstructionRra>();
    else if (OpcodePatternMatcher::Match(opcode, // 00XX 1001
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 0110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 0XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1100 0110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1110 1000
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '0'_b, '0'_b)))
             return make_shared<InstructionAdd>();
    else if (OpcodePatternMatcher::Match(opcode, // 110X X000
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1100 1001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '0'_b, '0'_b, '1'_b)))
             return make_shared<InstructionRet>();
    else if (OpcodePatternMatcher::Match(opcode, // 1100 1101
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 110X X100
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '0'_b)))
             return make_shared<InstructionCall>();
    else if (OpcodePatternMatcher::Match(opcode, // 1100 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return make_shared<InstructionAdc>();
    else if (OpcodePatternMatcher::Match(opcode, // 1001 0XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 0110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1101 0110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)))
             return make_shared<InstructionSub>();
    else if (OpcodePatternMatcher::Match(opcode, // 1101 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return make_shared<InstructionSbc>();
    else if (OpcodePatternMatcher::Match(opcode, // 1110 0110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 0110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 0XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
             return make_shared<InstructionAnd>();
    else if (OpcodePatternMatcher::Match(opcode, // 1111 0110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 0110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 0XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
             return make_shared<InstructionOr>();
    else if (OpcodePatternMatcher::Match(opcode, // 1110 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return make_shared<InstructionXor>();
    else if (OpcodePatternMatcher::Match(opcode, // 1111 1000
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b, '0'_b, '0'_b)))
             return make_shared<InstructionLdhl>();
    else if (OpcodePatternMatcher::Match(opcode, // 1111 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return make_shared<InstructionCp>();
    else if (OpcodePatternMatcher::Match(opcode, // 0011 0100
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX X100
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX 0011
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '1'_b, '1'_b)))
             return make_shared<InstructionInc>();
    else if (OpcodePatternMatcher::Match(opcode, // 0011 0101
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX X101
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX 1011
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '1'_b, '1'_b)))
             return make_shared<InstructionDec>();
    else if (OpcodePatternMatcher::Match(opcode, // 11XX 0101
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, 'X'_b, 'X'_b, '0'_b, '1'_b, '0'_b, '1'_b)))
             return make_shared<InstructionPush>();
    else if (OpcodePatternMatcher::Match(opcode, // 11XX 0001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '0'_b, '1'_b)))
             return make_shared<InstructionPop>();
    else if (OpcodePatternMatcher::Match(opcode, // 0000 1000
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '1'_b, '0'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX 0001
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 0011 0110
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 001X 1010
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, 'X'_b, '1'_b, '0'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 001X 0010
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, 'X'_b, '0'_b, '0'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX X110
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 000X 1010
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, 'X'_b, '1'_b, '0'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 000X 0010
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, 'X'_b, '0'_b, '0'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 0111 0XXX
             OpcodePatternMatcher::Pattern('0'_b, '1'_b, '1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 111X 0010
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, 'X'_b, '0'_b, '0'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 111X 0000
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, 'X'_b, '0'_b, '0'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 111X 1010
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, 'X'_b, '1'_b, '0'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1111 1001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 01XX X110
             OpcodePatternMatcher::Pattern('0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 01XX XXXX
             OpcodePatternMatcher::Pattern('0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b)))
             return make_shared<InstructionLd>();

    throw InstructionException("unknown instruction");
}

}