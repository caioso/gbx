#pragma once

#include <cstdlib>

namespace gbx::interfaces
{

enum class ResponseType
{
    Register,
    UnknownType
};

class DebugResponse
{
public:
    DebugResponse(ResponseType);
    virtual ~DebugResponse() = default;

protected:
    ResponseType _responseType;
};

}