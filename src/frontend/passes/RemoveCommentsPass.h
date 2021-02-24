#pragma once

#include <algorithm>
#include <string>

#include <iostream>

#include "../../interfaces/Pass.h"
#include "../../frontend/Lexemes.h"
#include "../../GBXAsmExceptions.h"

namespace gbxasm::frontend::passes
{

class RemoveCommentsPass : public interfaces::Pass
{
public:
    RemoveCommentsPass() = default;
    virtual ~RemoveCommentsPass() = default;

    virtual void Process(std::string) override;
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