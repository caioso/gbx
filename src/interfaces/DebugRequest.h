#pragma once

#include <cstdlib>

namespace gbx::interfaces
{

enum class RequestType
{
    UnknownType
};

class DebugRequest
{
public:
    DebugRequest(RequestType);

protected:
    RequestType _requestType;
};

}