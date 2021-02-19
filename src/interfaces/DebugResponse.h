#pragma once

#include <cstdlib>

namespace gbx::interfaces
{

enum class ResponseType
{
    UnknownType
};

class DebugResponse
{
public:
    DebugResponse(ResponseType);

protected:
    ResponseType _responseType;
};

}