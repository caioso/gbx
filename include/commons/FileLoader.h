#pragma once

#include <string>
#include <sstream>
#include <memory>

namespace gbxcommons
{

class FileLoader
{
public:
    FileLoader(std::string);
    ~FileLoader() = default;

    FileLoader(const FileLoader&) = default;
    FileLoader(FileLoader&&) = default;
    FileLoader& operator=(const FileLoader&) = default;
    FileLoader& operator=(FileLoader&&) = default;

    std::string FileName();
    void Load();
    std::shared_ptr<uint8_t[]> Bytes();

private:
    std::string _fileName{};
};

}