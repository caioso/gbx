#include "PackIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

PACKIntermediateRepresentation::PACKIntermediateRepresentation(std::string identifier, std::vector<DeclaredMember> members, size_t startLine, size_t startColumn, size_t endLine, size_t endColumn)
    : IntermediateRepresentation(ConstructionType::PACK, startLine, startColumn, endLine, endColumn)
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