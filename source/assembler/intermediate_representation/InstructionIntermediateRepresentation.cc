#include "InstructionIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

InstructionIntermediateRepresentation::InstructionIntermediateRepresentation( size_t startLine, size_t startColumn, size_t endLine, size_t endColumn)
    : IntermediateRepresentation(ConstructionType::Instruction, startLine, startColumn, endLine, endColumn)
{}

}