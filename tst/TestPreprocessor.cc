#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestUtils.h"

#include <memory>
#include <string>
#include <vector>

#include "../src/frontend/passes/ConditionalAssemblyPass.h"
#include "../src/frontend/PreProcessor.h"
#include "../src/interfaces/Pass.h"
#include "../src/GBXAsmExceptions.h"

using namespace gbxasm::interfaces;
using namespace gbxasm;
using namespace std;

class PreProcessorWrapper : public PreProcessor
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

    virtual void Process(string input) override
    {
        _passResult = input;
        _passResult += "source code";
    }

    [[nodiscard]] virtual string Result() override
    {
        return _passResult;
    }

private:
    string _passResult;
};

TEST(TestPreProcessor, Construction)
{
    auto preprocessor = make_shared<PreProcessor>();
}

TEST(TestPreProcessor, RegisterPass)
{
    vector<string> symbolTable;
    auto preprocessor = make_shared<PreProcessorWrapper>();
    auto conditionalAssemblyPass = make_shared<ConditionalAssemblyPass>(symbolTable);

    // Register This as the first pass.
    preprocessor->RegisterPass(conditionalAssemblyPass, 0);
    ASSERT_EQ(1llu, preprocessor->Passes().size());
}

TEST(TestPreProcessor, RegisterMultiplePasses)
{
    vector<string> symbolTable;
    auto preprocessor = make_shared<PreProcessorWrapper>();
    auto conditionalAssemblyPass = make_shared<ConditionalAssemblyPass>(symbolTable);
    auto dummyPass = make_shared<DummyPass>();

    // Register This as the first pass.
    preprocessor->RegisterPass(conditionalAssemblyPass, 0);
    preprocessor->RegisterPass(dummyPass, 1);
    
    ASSERT_EQ(2llu, preprocessor->Passes().size());
}

TEST(TestPreProcessor, RegisterPassOutOfBounds)
{
    vector<string> symbolTable;
    auto preprocessor = make_shared<PreProcessorWrapper>();
    auto conditionalAssemblyPass = make_shared<ConditionalAssemblyPass>(symbolTable);
    auto dummyPass = make_shared<DummyPass>();

    // Register This as the first pass.
    ASSERT_EXCEPTION( { preprocessor->RegisterPass(conditionalAssemblyPass, 100); }, 
                        PreProcessorException, 
                        "Attempted to register pass at index '100', which is out of bounds");
}

TEST(TestPreProcessor, ProcessPass)
{
    string inputCode = "this is my ";
    auto preprocessor = make_shared<PreProcessorWrapper>();
    auto dummyPass = make_shared<DummyPass>();

    // Register This as the first pass.
    preprocessor->RegisterPass(dummyPass, 0);
    auto result = preprocessor->ProcessPass(inputCode, 0);
    
    EXPECT_STREQ("this is my source code", result.c_str());
}

TEST(TestPreProcessor, ProcessInvalidPass)
{
    string inputCode = "this is my ";
    auto preprocessor = make_shared<PreProcessorWrapper>();
    auto dummyPass = make_shared<DummyPass>();

    // Register This as the first pass.
    preprocessor->RegisterPass(dummyPass, 0);

    ASSERT_EXCEPTION( { preprocessor->ProcessPass(inputCode, 1); }, 
                        PreProcessorException, 
                        "Attempted to process a pass with invalid index '1'");
}
