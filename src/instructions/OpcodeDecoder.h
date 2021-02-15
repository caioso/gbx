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
#include "InstructionLd.h"
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

#include "../GBXExceptions.h"
#include "../interfaces/InstructionInterface.h"
#include "InstructionUtilities.h"
#include "OpcodePatternMatcher.h"

namespace gbx
{

class OpcodeDecoder
{
public:
    OpcodeDecoder() = default;
    ~OpcodeDecoder() = default;

    [[nodiscard]] std::shared_ptr<interfaces::BaseInstructionInterface> DecodeOpcode(uint8_t, std::optional<uint8_t>);

private:
    std::shared_ptr<interfaces::BaseInstructionInterface> DecodeInstructionWithPreOpcode(uint8_t, std::optional<uint8_t>);
    std::shared_ptr<interfaces::BaseInstructionInterface> DecodeInstructionWithoutPreOpcode(uint8_t);
};

}