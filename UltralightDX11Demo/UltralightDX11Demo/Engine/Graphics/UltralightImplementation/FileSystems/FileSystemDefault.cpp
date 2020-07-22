#include "FileSystemDefault.h"
#include "../../../Utility/MimeTypeHelper.h"
#include "../../../Utility/DirectoryHelper.h"
#include <Ultralight\String.h>

FileSystemDefault::FileSystemDefault(std::string baseDirectory_)
{
	if (baseDirectory_ != "")
	{
		baseDirectory = baseDirectory_;
	}
	else
	{
		baseDirectory = DirectoryHelper::GetExecutableDirectoryA();
	}
}

FileSystemDefault::~FileSystemDefault()
{
}

bool FileSystemDefault::FileExists(const ul::String16& path_)
{
	std::string fullpath = GetRelativePath(path_);
	std::ifstream filestream(fullpath);
	return filestream.good();
}

bool FileSystemDefault::GetFileSize(ul::FileHandle handle_, int64_t& result_)
{
	auto iter = openFiles.find(handle_);
	if (iter != openFiles.end())
	{
		auto& file = iter->second;
		file->seekg(0, file->end);
		result_ = (int64_t)file->tellg();
		return true;
	}
	return false;
}

bool FileSystemDefault::GetFileMimeType(const ul::String16& path_, ul::String16& result_)
{
	ul::String8 utf8 = ul::String(path_).utf8();
	std::string filepath(utf8.data(), utf8.length());
	std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
	result_ = ul::String16(MimeTypeHelper::FileExtensionToMimeTypeA(ext.c_str()));
	return true;
}

ul::FileHandle FileSystemDefault::OpenFile(const ul::String16& path_, bool openForWriting_)
{
	std::string fullPath = GetRelativePath(path_);
	std::unique_ptr<std::ifstream> file(new std::ifstream(fullPath, std::ifstream::ate | std::ifstream::binary));
	if (!file->good())
		return ul::invalidFileHandle;

	ul::FileHandle handle = nextFileHandle++;
	openFiles[handle] = std::move(file);
	return handle;
}

void FileSystemDefault::CloseFile(ul::FileHandle& handle_)
{
	openFiles.erase(handle_);
	handle_ = ul::invalidFileHandle;
}

int64_t FileSystemDefault::ReadFromFile(ul::FileHandle handle_, char* data_, int64_t length_)
{
	auto iter = openFiles.find(handle_);
	if (iter != openFiles.end())
	{
		auto& file = iter->second;
		file->seekg(0, file->beg);
		file->read(data_, (std::streamsize)length_);
		return (int64_t)file->gcount();
	}
	return int64_t();
}

std::string FileSystemDefault::GetRelativePath(const ul::String16& filePath_)
{
	ul::String8 utf8 = ul::String(filePath_).utf8();
	std::string relativePath(utf8.data(), utf8.length());
	std::string fullpath = baseDirectory + relativePath;
	return fullpath;
}
