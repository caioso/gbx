#include "IntermediateRepresentation.h"

namespace gbxasm::intermediate_representation
{

IntermediateRepresentation::IntermediateRepresentation(ConstructionType type)
    : _type(type)
{}

ConstructionType IntermediateRepresentation::Type()
{
    return _type;
}
size_t IntermediateRepresentation::Column()
{
    return _column;
}

size_t IntermediateRepresentation::Line()
{
    return _line;
}

}