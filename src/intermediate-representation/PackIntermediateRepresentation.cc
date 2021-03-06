#include "PackIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

PackIntermediateRepresentation::PackIntermediateRepresentation(std::string identifier, std::vector<DeclaredMember> members, size_t line, size_t column)
    : IntermediateRepresentation(ConstructionType::Pack, line, column)
    , _identifier(identifier)
    , _members(members)
{}

string PackIntermediateRepresentation::Identifier()
{
    return _identifier;
}

vector<DeclaredMember>& PackIntermediateRepresentation::Members()
{
    return _members;
}
    
}