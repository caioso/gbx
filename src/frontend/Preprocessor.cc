#include "PreProcessor.h"

using namespace gbxasm::interfaces;
using namespace std;

namespace gbxasm
{

void PreProcessor::RegisterPass(shared_ptr<Pass> pass, size_t position)
{
    if (position > _passes.size())
    {
        stringstream ss;
        ss << "Attempted to register pass at index '" << position << "', which is out of bounds"; 
        throw PreProcessorException(ss.str());
    }

    _passes.insert(begin(_passes) + position, pass);
}

string PreProcessor::ProcessPass(string input, size_t passIndex)
{
    if (passIndex >= _passes.size())
    {
        stringstream ss;
        ss << "Attempted to process a pass with invalid index '" << passIndex << "'"; 
        throw PreProcessorException(ss.str());
    }

    _passes[passIndex]->Process(input);
    return _passes[passIndex]->Result();
}

}