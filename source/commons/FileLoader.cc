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
{}

shared_ptr<uint8_t[]> FileLoader::Bytes()
{
    return {};
}

}