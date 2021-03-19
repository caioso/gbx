#include "PackIntermediateRepresentation.h"

using namespace std;

namespace gbxasm::intermediate_representation
{

PackIntermediateRepresentation::PackIntermediateRepresentation(std::string identifier, std::vector<DeclaredMember> members, size_t line, size_t column)
    : IntermediateRepresentation(ConstructionType::PACK, line, column)
    , _identifier(identifier)
    , _members(members)
{}

string_view PackIntermediateRepresentation::Identifier()
{
    return _identifier;
}

vector<DeclaredMember>& PackIntermediateRepresentation::Members()
{
    return _members;
}
    
}