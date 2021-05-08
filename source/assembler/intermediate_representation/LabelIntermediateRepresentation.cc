#include "LabelIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

LabelIntermediateRepresentation::LabelIntermediateRepresentation(string identifier, LabelScope scope,  size_t startLine, size_t startColumn, size_t endLine, size_t endColumn)
    : IntermediateRepresentation(ConstructionType::Label, startLine, startColumn, endLine, endColumn)
    , _scope(scope)
    , _identifier(identifier)
{}

LabelScope LabelIntermediateRepresentation::Scope()
{
    return _scope;
}

string_view LabelIntermediateRepresentation::Identifier()
{
    return _identifier;
}

}