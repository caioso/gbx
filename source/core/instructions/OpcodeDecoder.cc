#include "OpcodeDecoder.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::instructions
{

BaseInstructionInterface* OpcodeDecoder::DecodeOpcode(uint8_t opcode, optional<uint8_t> preOpcode)
{
    if (preOpcode.has_value() && 
       (preOpcode.value() == InstructionConstants::PreOpcode_CB || 
        preOpcode.value() == InstructionConstants::PreOpcode_DD || 
        preOpcode.value() == InstructionConstants::PreOpcode_FD ||
        preOpcode.value() == InstructionConstants::PreOpcode_FC))
        return DecodeInstructionWithPreOpcode(opcode, preOpcode);
    else
        return DecodeInstructionWithoutPreOpcode(opcode);
}

BaseInstructionInterface* OpcodeDecoder::DecodeInstructionWithPreOpcode(uint8_t opcode, std::optional<uint8_t> preOpcode)
{
    if (preOpcode.value() == InstructionConstants::PreOpcode_CB)
    {
        if (OpcodePatternMatcher::Match(opcode, // 01XX XXXX
            OpcodePatternMatcher::Pattern('0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b)))
            return &_instructionBit;
        else if (OpcodePatternMatcher::Match(opcode, // 10XX XXXX
                OpcodePatternMatcher::Pattern('1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionRes;
        else if (OpcodePatternMatcher::Match(opcode, // 11XX XXXX
                OpcodePatternMatcher::Pattern('1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionSet;
        else if (OpcodePatternMatcher::Match(opcode, // 0000 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionRlc;
        else if (OpcodePatternMatcher::Match(opcode, // 0001 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionRl;
        else if (OpcodePatternMatcher::Match(opcode, // 0000 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionRrc;
        else if (OpcodePatternMatcher::Match(opcode, // 0001 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionRr;
        else if (OpcodePatternMatcher::Match(opcode, // 0010 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionSla;
        else if (OpcodePatternMatcher::Match(opcode, // 0010 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionSra;
        else if (OpcodePatternMatcher::Match(opcode, // 0011 1XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionSrl;
        else if (OpcodePatternMatcher::Match(opcode, // 0011 0XXX
                OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
                return &_instructionSwap;
    }
    else if (preOpcode.value() == InstructionConstants::PreOpcode_DD ||
             preOpcode.value() == InstructionConstants::PreOpcode_FD)
    {
        if (OpcodePatternMatcher::Match(opcode, // 01XX X110
            OpcodePatternMatcher::Pattern('0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '1'_b, '0'_b)) ||
            OpcodePatternMatcher::Match(opcode, // 0111 0XXX
            OpcodePatternMatcher::Pattern('0'_b, '1'_b, '1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
            return &_instructionLdAndLdu;
    }
    // [GBX ONLY] System-Mode Instruction
    else if (preOpcode.value() == InstructionConstants::PreOpcode_FC)
    {
        if (OpcodePatternMatcher::Match(opcode, // 1100 0011
            OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b)))
            return &_instructionJpu;
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
             return &_instructionLdAndLdu;
    }
    
    throw InstructionException("unknown instruction");
}

BaseInstructionInterface* OpcodeDecoder::DecodeInstructionWithoutPreOpcode(uint8_t opcode)
{
    if (OpcodePatternMatcher::Match(opcode, // 0000 0000
        OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '0'_b, '0'_b, '0'_b, '0'_b)))
        return &_instructionNop;
    else if (OpcodePatternMatcher::Match(opcode, // 0001 0000
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '0'_b, '0'_b, '0'_b, '0'_b)))
             return &_instructionStop;
    else if (OpcodePatternMatcher::Match(opcode, // 0000 0111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionRlca;
    else if (OpcodePatternMatcher::Match(opcode, // 0001 0111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionRla;
    else if (OpcodePatternMatcher::Match(opcode, // 0010 0111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionDaa;
    else if (OpcodePatternMatcher::Match(opcode, // 0010 1111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionCpl;
    else if (OpcodePatternMatcher::Match(opcode, // 0011 1111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionCcf;
    else if (OpcodePatternMatcher::Match(opcode, // 0011 0111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionScf;
    else if (OpcodePatternMatcher::Match(opcode, // 0001 1000
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 001X X000
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '0'_b)))
             return &_instructionJr;
    else if (OpcodePatternMatcher::Match(opcode, // 0111 0110
             OpcodePatternMatcher::Pattern('0'_b, '1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)))
             return &_instructionHalt;
    else if (OpcodePatternMatcher::Match(opcode, // 1110 1001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '0'_b, '1'_b)) || 
             OpcodePatternMatcher::Match(opcode, // 110X X01X
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, '0'_b, '1'_b, 'X'_b)))
             return &_instructionJp;
    else if (OpcodePatternMatcher::Match(opcode, // 0000 1111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionRrca;
    else if (OpcodePatternMatcher::Match(opcode, // 0001 1111
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionRra;
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
             return &_instructionAdd;
    else if (OpcodePatternMatcher::Match(opcode, // 110X X000
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1100 1001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '0'_b, '0'_b, '1'_b)))
             return &_instructionRet;
    else if (OpcodePatternMatcher::Match(opcode, // 1101 1001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b, '0'_b, '1'_b)))
             return &_instructionReti;
    else if (OpcodePatternMatcher::Match(opcode, // 11XX X111
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '1'_b, '1'_b)))
             return &_instructionRst;
    else if (OpcodePatternMatcher::Match(opcode, // 1100 1101
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 110X X100
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '0'_b)))
             return &_instructionCall; 
    else if (OpcodePatternMatcher::Match(opcode, // 1100 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return &_instructionAdc;
    else if (OpcodePatternMatcher::Match(opcode, // 1001 0XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 0110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1101 0110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)))
             return &_instructionSub;
    else if (OpcodePatternMatcher::Match(opcode, // 1101 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '0'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return &_instructionSbc;
    else if (OpcodePatternMatcher::Match(opcode, // 1110 0110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 0110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 0XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
             return &_instructionAnd;
    else if (OpcodePatternMatcher::Match(opcode, // 1111 0110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 0110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 0XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b)))
             return &_instructionOr;
    else if (OpcodePatternMatcher::Match(opcode, // 1110 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '0'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return &_instructionXor;
    else if (OpcodePatternMatcher::Match(opcode, // 1111 1000
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b, '0'_b, '0'_b)))
             return &_instructionLdhl;
    else if (OpcodePatternMatcher::Match(opcode, // 1111 1011
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b, '1'_b, '1'_b)))
             return &_instructionEi;
    else if (OpcodePatternMatcher::Match(opcode, // 1111 0011
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '0'_b, '0'_b, '1'_b, '1'_b)))
             return &_instructionDi;
    else if (OpcodePatternMatcher::Match(opcode, // 1111 1110
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 1110
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '1'_b, '1'_b, '1'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 1XXX
             OpcodePatternMatcher::Pattern('1'_b, '0'_b, '1'_b, '1'_b, '1'_b, 'X'_b, 'X'_b, 'X'_b)))
             return &_instructionCp;
    else if (OpcodePatternMatcher::Match(opcode, // 0011 0100
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX X100
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '0'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX 0011
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '1'_b, '1'_b)))
             return &_instructionInc;
    else if (OpcodePatternMatcher::Match(opcode, // 0011 0101
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, '1'_b, '1'_b, '0'_b, '1'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX X101
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '1'_b)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX 1011
             OpcodePatternMatcher::Pattern('0'_b, '0'_b, 'X'_b, 'X'_b, '1'_b, '0'_b, '1'_b, '1'_b)))
             return &_instructionDec;
    else if (OpcodePatternMatcher::Match(opcode, // 11XX 0101
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, 'X'_b, 'X'_b, '0'_b, '1'_b, '0'_b, '1'_b)))
             return &_instructionPush;
    else if (OpcodePatternMatcher::Match(opcode, // 11XX 0001
             OpcodePatternMatcher::Pattern('1'_b, '1'_b, 'X'_b, 'X'_b, '0'_b, '0'_b, '0'_b, '1'_b)))
             return &_instructionPop;
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
             return &_instructionLdAndLdu;

    throw InstructionException("unknown instruction");
}

}