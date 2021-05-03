#include "InstructionIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

InstructionIntermediateRepresentation::InstructionIntermediateRepresentation(size_t line, size_t column)
    : IntermediateRepresentation(ConstructionType::Instruction, line, column)
{}

}