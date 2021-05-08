#include "IntermediateRepresentation.h"

namespace gbxasm::intermediate_representation
{

IntermediateRepresentation::IntermediateRepresentation(ConstructionType type, size_t startLine, size_t startColumn, size_t endLine, size_t endColumn)
    : _type(type)
    , _startLine(startLine)
    , _startColumn(startColumn)
    , _endLine(endLine)
    , _endColumn(endColumn)
{}

ConstructionType IntermediateRepresentation::Type()
{
    return _type;
}

size_t IntermediateRepresentation::StartColumn()
{
    return _startColumn;
}

size_t IntermediateRepresentation::StartLine()
{
    return _startLine;
}

size_t IntermediateRepresentation::EndColumn()
{
    return _endColumn;
}

size_t IntermediateRepresentation::EndLine()
{
    return _endLine;
}

}