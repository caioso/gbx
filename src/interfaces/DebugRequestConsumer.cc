#include "DebugRequestConsumer.h"
#include "DebugRequestProducer.h"

using namespace std;

namespace gbx::interfaces
{

DebugRequestConsumer::DebugRequestConsumer(std::shared_ptr<DebugRequestProducer> producer)
    : _producer(producer)
{}

}