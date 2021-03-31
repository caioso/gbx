#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <memory>
#include <string>

#include "TestUtils.h"

#include "GBXCommonsExceptions.h"
#include "FileLoader.h"

using namespace std;
using namespace gbxcommons;

using ::testing::Return;
using ::testing::_;

std::string SampleBinTestFileName()
{
    return GBXTestEnvironment::TestDataPath + "sample.bin";
}

std::string InvalidFileName()
{
    return GBXTestEnvironment::TestDataPath + "nofile.bin";
}

TEST(CommonsTests_FileLoader, Construction)
{
    auto loader = make_shared<FileLoader>(SampleBinTestFileName());
    EXPECT_STREQ(SampleBinTestFileName().c_str(), loader->FileName().c_str());
}

TEST(CommonsTests_FileLoader, LoadFile)
{
    auto loader = make_shared<FileLoader>(SampleBinTestFileName());

    loader->Load();
    auto [buffer, size] = loader->FileData();
    
    // File Size
    EXPECT_EQ(16llu, size);

    // file content
    auto expectedContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    auto bufferPosition = 0llu;
    
    for (auto byte : expectedContent)
        EXPECT_EQ(static_cast<uint8_t>(byte), buffer[bufferPosition++]);
}

TEST(CommonsTests_FileLoader, LoadInvalidFile)
{
    auto loader = make_shared<FileLoader>(InvalidFileName());

    ASSERT_EXCEPTION( { loader->Load(); }, 
                      FileLoaderException, 
                      "Unable to load file './build/test/test_data/nofile.bin'");
}

TEST(CommonsTests_FileLoader, TryToGetFileDataWithoutLoadingFile)
{
    auto loader = make_shared<FileLoader>(InvalidFileName());

    ASSERT_EXCEPTION( { loader->FileData(); }, 
                      FileLoaderException, 
                      "File content has not yet been loaded");
}
