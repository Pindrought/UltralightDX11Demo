#pragma once
#include <string>
class DirectoryHelper
{
public:
	static std::string GetExecutableDirectoryA();
	static std::wstring GetExecutableDirectory();
	static std::string NormalizePathA(std::string path);
	static std::wstring NormalizePath(std::wstring path);

private:
	static std::string executableDirectoryA;
	static std::wstring executableDirectory;
};