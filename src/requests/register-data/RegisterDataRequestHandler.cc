#include "RegisterDataRequestHandler.h"

using namespace gbx;
using namespace gbx::interfaces;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbx::requests
{

shared_ptr<DebugMessage> RegisterDataRequestHandler::Process(shared_ptr<DebugMessage> request, shared_ptr<Runtime> runtime)
{
    auto registerRequest = dynamic_pointer_cast<RegisterDataRequest>(request);

    if (registerRequest == nullptr)
        throw RequestHandlerException("Incorrect request type received by 'RegisterDataRequestHandler'");

    if (registerRequest->Operation() == RegisterDataOperation::Read)
    {
        auto registerValue = runtime->ReadRegister(registerRequest->Register());
        return make_shared<RegisterDataResponse>(registerRequest->Register(), registerValue);
    }
    else if (registerRequest->Operation() == RegisterDataOperation::Write)
    {
        runtime->WriteRegister(registerRequest->Register(), registerRequest->Value());
        return make_shared<RegisterDataResponse>(registerRequest->Register(), registerRequest->Value());
    }
    
    return make_shared<DebugMessage>(MessageType::UnknownMessage);
}

}