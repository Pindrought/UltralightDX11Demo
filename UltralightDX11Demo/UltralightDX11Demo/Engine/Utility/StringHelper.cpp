#include "StringHelper.h"

std::wstring StringHelper::StringToWide(std::string str_)
{
	std::wstring wide_string(str_.begin(), str_.end());
	return wide_string;
}
