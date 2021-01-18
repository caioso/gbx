#pragma once

#include <memory>
#include <optional>

#include "InstructionAdc.h"
#include "InstructionAdd.h"
#include "InstructionAnd.h"
#include "InstructionCp.h"
#include "InstructionDec.h"
#include "InstructionInc.h"
#include "InstructionLd.h"
#include "InstructionLdhl.h"
#include "InstructionOr.h"
#include "InstructionPop.h"
#include "InstructionPush.h"
#include "InstructionRla.h"
#include "InstructionRlc.h"
#include "InstructionRlca.h"
#include "InstructionRra.h"
#include "InstructionRrca.h"
#include "InstructionSub.h"
#include "InstructionSbc.h"
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

    [[nodiscard]] std::shared_ptr<interfaces::InstructionInterface> DecodeOpcode(uint8_t, std::optional<uint8_t>);

private:
    std::shared_ptr<interfaces::InstructionInterface> DecodeInstructionWithPreOpcode(uint8_t, std::optional<uint8_t>);
    std::shared_ptr<interfaces::InstructionInterface> DecodeInstructionWithoutPreOpcode(uint8_t);
};

}