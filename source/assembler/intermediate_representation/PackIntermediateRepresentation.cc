#include "PACKIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

PACKIntermediateRepresentation::PACKIntermediateRepresentation(std::string identifier, std::vector<DeclaredMember> members, size_t line, size_t column)
    : IntermediateRepresentation(ConstructionType::PACK, line, column)
    , _identifier(identifier)
    , _members(members)
{}

string_view PACKIntermediateRepresentation::Identifier()
{
    return _identifier;
}
 
vector<DeclaredMember>& PACKIntermediateRepresentation::Members()
{
    return _members;
}
    
}