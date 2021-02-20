#include "RegisterDataRequestHandler.h"

using namespace gbx;
using namespace gbx::interfaces;
using namespace std;

namespace gbx::requests
{

shared_ptr<DebugResponse> RegisterDataRequestHandler::Process(shared_ptr<DebugRequest> request, shared_ptr<Runtime> runtime)
{
    auto response = make_shared<DebugResponse>(ResponseType::UnknownType);
    return response;
}

}