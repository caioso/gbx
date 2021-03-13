#include "RemoveCommentsPass.h"

using std::string;

namespace gbxasm::frontend::passes
{

void RemoveCommentsPass::Process(string input)
{
    ClearResult();
    _result = input;

    RemoveMultipleLineComment();
    RemoveSingleLineComment();
}

void RemoveCommentsPass::RemoveSingleLineComment()
{
    size_t endPosition = 0;
    while (endPosition != string::npos || endPosition < _result.size())
    {
        auto stringStart = _result.find("\"", endPosition);
        auto charStart = _result.find("'", endPosition);
        auto commentStart = _result.find(Lexemes::PreProcessorSingleLineCommentMarker, endPosition);

        auto targetCheck = std::min(stringStart, std::min(charStart, commentStart));

        if (targetCheck == stringStart && stringStart != string::npos)
            SkipCharacters("\"", stringStart, endPosition);
        else if (targetCheck == charStart && charStart != string::npos)
            SkipCharacters("\'", charStart, endPosition);
        else
            ClearSingleLineComment(commentStart, endPosition);
    }
}

void RemoveCommentsPass::ClearSingleLineComment(size_t commentStart, size_t& endPosition)
{
    endPosition = _result.find("\n", commentStart);
    ClearCommentSection(commentStart, endPosition, 1);
}

void RemoveCommentsPass::RemoveMultipleLineComment()
{
    size_t endPosition = 0;
    while (endPosition != string::npos || endPosition < _result.size())
    {
        auto stringStart = _result.find("\"", endPosition);
        auto charStart = _result.find("'", endPosition);
        auto commentStart = _result.find(Lexemes::PreProcessorMultipleLineCommentMarkerStarter, endPosition);

        auto targetCheck = std::min(stringStart, std::min(charStart, commentStart));

        if (targetCheck == stringStart && stringStart != string::npos)
            SkipCharacters("\"", stringStart, endPosition);
        else if (targetCheck == charStart && charStart != string::npos)
            SkipCharacters("\'", charStart, endPosition);
        else
            ClearMultipleLineComment(commentStart, endPosition);
    }
}

void RemoveCommentsPass::ClearMultipleLineComment(size_t commentStart, size_t& endPosition)
{
    auto originalStart = endPosition;
    endPosition = _result.find(Lexemes::PreProcessorMultipleLineCommentMarkerEnder, commentStart == string::npos ? originalStart : commentStart);
    
    if (commentStart != string::npos && endPosition == string::npos)
        throw PreProcessorException("Expected ':;' in multiple line comment");
    
    if (commentStart == string::npos && endPosition != string::npos)
        throw PreProcessorException("Expected ';:' in multiple line comment");

    ClearCommentSection(commentStart, endPosition, Lexemes::PreProcessorMultipleLineCommentMarkerEnder.size());
}

void RemoveCommentsPass::SkipCharacters(string maker, size_t startPosition, size_t& endPosition)
{
    endPosition = _result.find(maker, startPosition + 1);
    endPosition = endPosition == string::npos? string::npos : endPosition + 1;
}

void RemoveCommentsPass::ClearCommentSection(size_t startPosition, size_t endPosition, size_t enderSize)
{
    for (auto i = startPosition; i < (endPosition == string::npos? _result.size() : endPosition) + enderSize; ++i)
        if (_result[i] != '\n')
            _result[i] = ' ';
}

string RemoveCommentsPass::Result()
{
    if (_result.empty())
        throw PreProcessorException("Attempted to read a result without calling 'Process'");

    return _result;
}

void RemoveCommentsPass::ClearResult()
{
    _result.clear();
}
   
}