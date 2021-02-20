#pragma once

#include <cstdlib>

namespace gbx::interfaces
{

enum class RequestType
{
    // Data Requests
    Register,
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