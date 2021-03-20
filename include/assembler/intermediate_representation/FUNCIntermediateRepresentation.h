#pragma once

#include <string>
#include <vector>

#include "DeclaredMember.h"
#include "IntermediateRepresentation.h"
#include "Token.h"

namespace gbxasm::intermediate_representation
{
class FUNCIntermediateRepresentation : public IntermediateRepresentation
{
public:
    FUNCIntermediateRepresentation(std::string, std::vector<gbxasm::frontend::Token>, std::vector<DeclaredArgument>, std::vector<DeclaredArgument>, size_t, size_t);
    ~FUNCIntermediateRepresentation() = default;

    FUNCIntermediateRepresentation(const FUNCIntermediateRepresentation&) = default;
    FUNCIntermediateRepresentation(FUNCIntermediateRepresentation&&) = default;
    FUNCIntermediateRepresentation& operator=(const FUNCIntermediateRepresentation&) = default;
    FUNCIntermediateRepresentation& operator=(FUNCIntermediateRepresentation&&) = default;

    std::string_view Identifier();
    std::vector<gbxasm::frontend::Token>& BodyTokens();

    std::vector<DeclaredArgument>& InputArguments();
    std::vector<DeclaredArgument>& OutputArguments();

private:
    std::vector<DeclaredArgument> _inputArguments{};
    std::vector<DeclaredArgument> _outputArguments{};
    std::vector<gbxasm::frontend::Token> _bodyTokens{};
    std::string _identifier{};
};

}