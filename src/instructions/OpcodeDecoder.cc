#include "OpcodeDecoder.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx
{

shared_ptr<InstructionInterface> OpcodeDecoder::DecodeOpcode(uint8_t opcode, optional<uint8_t> preOpcode)
{
    if (preOpcode.has_value() && 
       (preOpcode.value() == InstructionConstants::PreOpcode_DD || 
        preOpcode.value() == InstructionConstants::PreOpcode_FD))
        return DecodeInstructionWithPreOpcode(opcode, preOpcode);
    else
        return DecodeInstructionWithoutPreOpcode(opcode);
}

shared_ptr<InstructionInterface> OpcodeDecoder::DecodeInstructionWithPreOpcode(__attribute__((unused)) uint8_t opcode,__attribute__((unused)) std::optional<uint8_t> preOpcode)
{
    if (OpcodePatternMatcher::Match(opcode, // 01XX X110
        OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_X, b::_X, b::_X, b::_1, b::_1, b::_0)) ||
        OpcodePatternMatcher::Match(opcode, // 0111 0XXX
        OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_1, b::_1, b::_0, b::_X, b::_X, b::_X)))
        return make_shared<InstructionLd>();

    throw InstructionException("unknown instruction");
}

shared_ptr<InstructionInterface> OpcodeDecoder::DecodeInstructionWithoutPreOpcode(uint8_t opcode)
{
    // Add Instruction Opcodes
    if (OpcodePatternMatcher::Match(opcode, // 1000 0110
        OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_0, b::_0, b::_1, b::_1, b::_0)) ||
        OpcodePatternMatcher::Match(opcode, // 1000 0XXX
        OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_0, b::_0, b::_X, b::_X, b::_X)) ||
        OpcodePatternMatcher::Match(opcode, // 1100 0110
        OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_0, b::_0, b::_0, b::_1, b::_1, b::_0)))
        return make_shared<InstructionAdd>();
    else if (OpcodePatternMatcher::Match(opcode, // 1100 1110
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_0, b::_0, b::_1, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 1110
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_0, b::_1, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1000 1XXX
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_0, b::_1, b::_X, b::_X, b::_X)))
             return make_shared<InstructionAdc>();
    else if (OpcodePatternMatcher::Match(opcode, // 1001 0XXX
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_1, b::_0, b::_X, b::_X, b::_X)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 0110
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_1, b::_0, b::_X, b::_X, b::_X)) ||
             OpcodePatternMatcher::Match(opcode, // 1101 0110
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_0, b::_1, b::_0, b::_1, b::_1, b::_0)))
             return make_shared<InstructionSub>();
    else if (OpcodePatternMatcher::Match(opcode, // 1101 1110
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_0, b::_1, b::_1, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 1110
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_1, b::_1, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1001 1XXX
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_0, b::_1, b::_1, b::_X, b::_X, b::_X)))
             return make_shared<InstructionSbc>();
    else if (OpcodePatternMatcher::Match(opcode, // 1110 0110
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_0, b::_0, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 0110
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_1, b::_0, b::_0, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 0XXX
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_1, b::_0, b::_0, b::_X, b::_X, b::_X)))
             return make_shared<InstructionAnd>();
    else if (OpcodePatternMatcher::Match(opcode, // 1111 0110
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_1, b::_0, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 0110
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_1, b::_1, b::_0, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1011 0XXX
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_1, b::_1, b::_0, b::_X, b::_X, b::_X)))
             return make_shared<InstructionOr>();
    else if (OpcodePatternMatcher::Match(opcode, // 1110 1110
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_0, b::_1, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 1110
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_1, b::_0, b::_1, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1010 1XXX
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_1, b::_0, b::_1, b::_X, b::_X, b::_X)))
             return make_shared<InstructionXor>();
    else if (OpcodePatternMatcher::Match(opcode, // 1011 1XXX
             OpcodePatternMatcher::Pattern(b::_1, b::_0, b::_1, b::_1, b::_1, b::_X, b::_X, b::_X)))
             return make_shared<InstructionCp>();
    else if (OpcodePatternMatcher::Match(opcode, // 00XX 0001
             OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_X, b::_X, b::_0, b::_0, b::_0, b::_1)) ||
             OpcodePatternMatcher::Match(opcode, // 0011 0110
             OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_1, b::_1, b::_0, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 001X 1010
             OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_1, b::_X, b::_1, b::_0, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 001X 0010
             OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_1, b::_X, b::_0, b::_0, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 00XX X110
             OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_X, b::_X, b::_X, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 000X 1010
             OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_0, b::_X, b::_1, b::_0, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 000X 0010
             OpcodePatternMatcher::Pattern(b::_0, b::_0, b::_0, b::_X, b::_0, b::_0, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 0111 0XXX
             OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_1, b::_1, b::_0, b::_X, b::_X, b::_X)) ||
             OpcodePatternMatcher::Match(opcode, // 111X 0010
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_X, b::_0, b::_0, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 111X 0000
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_X, b::_0, b::_0, b::_0, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 111X 1010
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_X, b::_1, b::_0, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 1111 1001
             OpcodePatternMatcher::Pattern(b::_1, b::_1, b::_1, b::_1, b::_1, b::_0, b::_0, b::_1)) ||
             OpcodePatternMatcher::Match(opcode, // 01XX X110
             OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_X, b::_X, b::_X, b::_1, b::_1, b::_0)) ||
             OpcodePatternMatcher::Match(opcode, // 01XX XXXX
             OpcodePatternMatcher::Pattern(b::_0, b::_1, b::_X, b::_X, b::_X, b::_X, b::_X, b::_X)))
             return make_shared<InstructionLd>();

    throw InstructionException("unknown instruction");
}

}