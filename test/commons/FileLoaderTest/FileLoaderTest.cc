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

void CreateFile()
{
    // Save Temp File Here. Delete it afterwards
}

TEST(CommonsTests_FileLoader, Construction)
{
    auto loader = make_shared<FileLoader>("filename");
    EXPECT_STREQ("filename", loader->FileName().c_str());
}

TEST(CommonsTests_FileLoader, LoadFile)
{
    auto loader = make_shared<FileLoader>("filename");
    CreateFile();

    loader->Load();
    auto buffer = loader->Bytes();
}

// Make test for exception when trying to read bytes of a non-loaded fileLoader