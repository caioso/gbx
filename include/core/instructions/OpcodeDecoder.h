#pragma once

#include <memory>
#include <optional>

#include "InstructionAdc.h"
#include "InstructionAdd.h"
#include "InstructionAnd.h"
#include "InstructionBit.h"
#include "InstructionCall.h"
#include "InstructionCcf.h"
#include "InstructionCp.h"
#include "InstructionCpl.h"
#include "InstructionDaa.h"
#include "InstructionDec.h"
#include "InstructionDi.h"
#include "InstructionEi.h"
#include "InstructionInc.h"
#include "InstructionHalt.h"
#include "InstructionJp.h"
#include "InstructionJr.h"
#include "InstructionLdAndLdu.h"
#include "InstructionLdhl.h"
#include "InstructionNop.h"
#include "InstructionOr.h"
#include "InstructionPop.h"
#include "InstructionPush.h"
#include "InstructionRes.h"
#include "InstructionRet.h"
#include "InstructionReti.h"
#include "InstructionRst.h"
#include "InstructionRl.h"
#include "InstructionRla.h"
#include "InstructionRlc.h"
#include "InstructionRlca.h"
#include "InstructionRr.h"
#include "InstructionRra.h"
#include "InstructionRrc.h"
#include "InstructionRrca.h"
#include "InstructionSbc.h"
#include "InstructionScf.h"
#include "InstructionSet.h"
#include "InstructionSla.h"
#include "InstructionSra.h"
#include "InstructionSrl.h"
#include "InstructionSub.h"
#include "InstructionStop.h"
#include "InstructionSwap.h"
#include "InstructionXor.h"

#include "InstructionJpu.h"

#include "GBXCoreExceptions.h"
#include "InstructionInterface.h"
#include "InstructionUtilities.h"
#include "OpcodePatternMatcher.h"

namespace gbxcore::instructions
{

class OpcodeDecoder
{
public:
    OpcodeDecoder() = default;
    ~OpcodeDecoder() = default;

    [[nodiscard]] interfaces::BaseInstructionInterface* DecodeOpcode(uint8_t, std::optional<uint8_t>);

private:
    interfaces::BaseInstructionInterface* DecodeInstructionWithPreOpcode(uint8_t, std::optional<uint8_t>);
    interfaces::BaseInstructionInterface* DecodeInstructionWithoutPreOpcode(uint8_t);

    InstructionBit _instructionBit;
    InstructionRes _instructionRes;
    InstructionSet _instructionSet;
    InstructionRlc _instructionRlc;
    InstructionRl _instructionRl;
    InstructionRrc _instructionRrc;
    InstructionRr _instructionRr;
    InstructionSla _instructionSla;
    InstructionSra _instructionSra;
    InstructionSrl _instructionSrl;
    InstructionSwap _instructionSwap;
    InstructionLdAndLdu _instructionLdAndLdu;
    InstructionJpu _instructionJpu;
    InstructionNop _instructionNop;
    InstructionStop _instructionStop;
    InstructionRlca _instructionRlca;
    InstructionRla _instructionRla;
    InstructionDaa _instructionDaa;
    InstructionCpl _instructionCpl;
    InstructionCcf _instructionCcf;
    InstructionScf _instructionScf;
    InstructionJr _instructionJr;
    InstructionHalt _instructionHalt;
    InstructionJp _instructionJp;
    InstructionRrca _instructionRrca;
    InstructionRra _instructionRra;
    InstructionAdd _instructionAdd;
    InstructionRet _instructionRet;
    InstructionReti _instructionReti;
    InstructionRst _instructionRst;
    InstructionCall _instructionCall;
    InstructionAdc _instructionAdc;
    InstructionSub _instructionSub;
    InstructionSbc _instructionSbc;
    InstructionAnd _instructionAnd;
    InstructionOr _instructionOr;
    InstructionXor _instructionXor;
    InstructionLdhl _instructionLdhl;
    InstructionEi _instructionEi;
    InstructionDi _instructionDi;
    InstructionCp _instructionCp;
    InstructionInc _instructionInc;
    InstructionDec _instructionDec;
    InstructionPush _instructionPush;
    InstructionPop _instructionPop;
};

}