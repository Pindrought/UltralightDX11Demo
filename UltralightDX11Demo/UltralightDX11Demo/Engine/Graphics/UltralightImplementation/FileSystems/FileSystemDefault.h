#pragma once
#include "../../../../pch.h"

namespace ul = ultralight;

class FileSystemDefault : public ul::FileSystem
{
public:
    FileSystemDefault(std::string baseDir = "");
    virtual ~FileSystemDefault();
    virtual bool FileExists(const ul::String16& path) override;
    virtual bool GetFileSize(ul::FileHandle handle, int64_t& result) override;
    virtual bool GetFileMimeType(const ul::String16& path, ul::String16& result) override;
    virtual ul::FileHandle OpenFile(const ul::String16& path, bool openForWriting) override;
    virtual void CloseFile(ul::FileHandle& handle) override;
    virtual int64_t ReadFromFile(ul::FileHandle handle, char* data, int64_t length) override;
private:
    std::string GetRelativePath(const ul::String16& filePath);
    std::string baseDirectory = "";
    ul::FileHandle nextFileHandle = 0;
    std::map<ul::FileHandle, std::unique_ptr<std::ifstream>> openFiles;
};