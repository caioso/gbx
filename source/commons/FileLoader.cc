#include "FileLoader.h"

using namespace std;

namespace gbxcommons
{

FileLoader::FileLoader(string fileName)
    : _fileName(fileName)
{}

string FileLoader::FileName()
{
    return _fileName;
}

void FileLoader::Load()
{
    AcquireFileInfo();
    AcquireFileContent();
}

void FileLoader::AcquireFileInfo()
{
    ifstream queryStream(_fileName, ios::ate | ios::binary);
    
    if (!queryStream)
    {
        stringstream ss;
        ss << "Unable to load file '" << _fileName << "'";
        throw FileLoaderException(ss.str());
    }

    // acquire file size
    _fileSize = queryStream.tellg();
    queryStream.close();
}

void FileLoader::AcquireFileContent()
{
    _fileContent = make_unique<uint8_t[]>(_fileSize);
    ifstream fileStream(_fileName, ios::out | ios::binary);
    
    fileStream.read(reinterpret_cast<char*>(_fileContent.get()), _fileSize);
    fileStream .close();
}

tuple<std::unique_ptr<uint8_t[]>, size_t> FileLoader::FileData()
{
    if (_fileContent == nullptr)
        throw FileLoaderException("File content has not yet been loaded");

    return make_tuple(std::move(_fileContent), _fileSize);
}

}