#include "RegisterDataRequestHandler.h"

using namespace gbx;
using namespace gbx::interfaces;
using namespace std;

namespace gbx::requests
{

shared_ptr<DebugResponse> RegisterDataRequestHandler::Process(shared_ptr<DebugRequest> request, shared_ptr<Runtime> runtime)
{
    auto registerRequest = dynamic_pointer_cast<RegisterDataRequest>(request);

    if (registerRequest == nullptr)
        throw RequestHandlerException("Incorrect request type received by 'RegisterDataRequestHandler'");

    if (registerRequest->Operation() == RegisterDataOperation::Read)
    {
        auto registerValue = runtime->ReadRegister(registerRequest->Register());
        return make_shared<RegisterDataResponse>(registerRequest->Register(), registerValue);
    }
    
    return make_shared<DebugResponse>(ResponseType::UnknownType);
}

}