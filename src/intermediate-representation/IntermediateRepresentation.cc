#include "IntermediateRepresentation.h"

namespace gbxasm::intermediate_representation
{

IntermediateRepresentation::IntermediateRepresentation(ConstructionType type, size_t line, size_t column)
    : _type(type)
    , _line(line)
    , _column(column)
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