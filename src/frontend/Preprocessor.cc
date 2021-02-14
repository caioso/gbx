#include "Preprocessor.h"

using namespace gbxasm::interfaces;
using namespace std;

namespace gbxasm
{

void Preprocessor::RegisterPass(shared_ptr<Pass> pass, size_t position)
{
    if (position >= _passes.size() + 1)
    {
        stringstream ss;
        ss << "Attempted to register pass at index '" << position << "', which is out of bounds"; 
        throw PreprocessorException(ss.str());
    }

    _passes.insert(begin(_passes) + position, pass);
}

void Preprocessor::ProcessPass(size_t passIndex)
{

}

}