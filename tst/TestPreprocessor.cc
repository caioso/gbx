#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>

#include "../src/frontend/passes/ConditionalAssemblyPass.h"
#include "../src/frontend/Preprocessor.h"
#include "../src/interfaces/Pass.h"
#include "../src/GBXAsmExceptions.h"

using namespace gbxasm::interfaces;
using namespace gbxasm;
using namespace std;

class PreprocessorWrapper : public Preprocessor
{
public:
    vector<shared_ptr<Pass>>& Passes()
    {
        return _passes;
    }
};

class DummyPass : public Pass
{
public:
    DummyPass() = default;
    virtual ~DummyPass() = default;

    virtual void Process(__attribute__((unused)) std::string input) override
    {}

    virtual std::string Result() override
    {
        return string("");
    }
};

TEST(TestPreprocessor, Construction)
{
    auto preprocessor = make_shared<Preprocessor>();
}

TEST(TestPreprocessor, RegisterPass)
{
    auto preprocessor = make_shared<PreprocessorWrapper>();
    auto conditionalAssemblyPass = make_shared<ConditionalAssemblyPass>();

    // Register This as the first pass.
    preprocessor->RegisterPass(conditionalAssemblyPass, 0);
    ASSERT_EQ(1llu, preprocessor->Passes().size());
}

TEST(TestPreprocessor, RegisterMultiplePasses)
{
    auto preprocessor = make_shared<PreprocessorWrapper>();
    auto conditionalAssemblyPass = make_shared<ConditionalAssemblyPass>();
    auto dummyPass = make_shared<DummyPass>();

    // Register This as the first pass.
    preprocessor->RegisterPass(conditionalAssemblyPass, 0);
    preprocessor->RegisterPass(dummyPass, 1);
    
    ASSERT_EQ(2llu, preprocessor->Passes().size());
}

TEST(TestPreprocessor, RegisterPassOutOfBounds)
{
    auto preprocessor = make_shared<PreprocessorWrapper>();
    auto conditionalAssemblyPass = make_shared<ConditionalAssemblyPass>();
    auto dummyPass = make_shared<DummyPass>();

    // Register This as the first pass.
    ASSERT_EXCEPTION( { preprocessor->RegisterPass(conditionalAssemblyPass, 100); }, 
                        PreprocessorException, 
                        "Attempted to register pass at index '100', which is out of bounds");
}

TEST(TestPreprocessor, ProcessPass)
{
    auto preprocessor = make_shared<PreprocessorWrapper>();
    auto dummyPass = make_shared<DummyPass>();

    // Register This as the first pass.
    preprocessor->RegisterPass(dummyPass, 0);
    preprocessor->ProcessPass(0);
    
}
