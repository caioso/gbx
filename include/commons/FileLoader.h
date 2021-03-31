#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <tuple>
#include <utility>

#include "GBXCommonsExceptions.h"

namespace gbxcommons
{

class FileLoader
{
public:
    FileLoader(std::string);
    ~FileLoader() = default;

    FileLoader(const FileLoader&) = delete;
    FileLoader(FileLoader&&) = default;
    FileLoader& operator=(const FileLoader&) = delete;
    FileLoader& operator=(FileLoader&&) = default;

    std::string FileName();
    void Load();
    std::tuple<std::unique_ptr<uint8_t[]>, size_t> FileData();

private:
    void AcquireFileInfo();
    void AcquireFileContent();

    std::string _fileName{};
    size_t _fileSize{};
    std::unique_ptr<uint8_t[]> _fileContent;
};

}