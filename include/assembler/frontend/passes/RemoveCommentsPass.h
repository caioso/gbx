#pragma once

#include <algorithm>
#include <string>

#include <iostream>

#include "GBXAsmExceptions.h"
#include "Lexemes.h"
#include "Pass.h"

namespace gbxasm::frontend::passes
{

class RemoveCommentsPass : public interfaces::Pass
{
public:
    RemoveCommentsPass() = default;
    virtual ~RemoveCommentsPass() = default;

    RemoveCommentsPass(const RemoveCommentsPass&) = default;
    RemoveCommentsPass(RemoveCommentsPass&&) = default;
    RemoveCommentsPass& operator=(const RemoveCommentsPass&) = default;
    RemoveCommentsPass& operator=(RemoveCommentsPass&&) = default;

    void Process(std::string) override;
    [[nodiscard]] virtual std::string Result() override;

private:
    void ClearResult();
    void RemoveSingleLineComment();
    void RemoveMultipleLineComment();
    void ClearCommentSection(size_t, size_t, size_t);
    void SkipCharacters(std::string, size_t, size_t&);
    void ClearSingleLineComment(size_t, size_t&);
    void ClearMultipleLineComment(size_t, size_t&);
    std::string _result{};
};

}