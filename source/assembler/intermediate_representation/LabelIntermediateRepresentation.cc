#include "LabelIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

LabelIntermediateRepresentation::LabelIntermediateRepresentation(string identifier, LabelScope scope, size_t line, size_t column)
    : IntermediateRepresentation(ConstructionType::Label, line, column)
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