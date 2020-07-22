#include "ErrorHandler.h"
#include <comdef.h>

void ErrorHandler::LogCriticalError(const std::string& msg_)
{
	MessageBoxA(NULL, msg_.c_str(), "Critical Error", 0);
}

void ErrorHandler::LogCriticalError(HRESULT hr_, const std::string& msg_, const std::string& file_, const std::string& function_, int line_)
{
	_com_error error(hr_);
	std::wstring errorMessage = StringHelper::StringToWide(msg_);
	errorMessage += L"\n";
	errorMessage += error.ErrorMessage();

	std::string errorLocation = "\n\nFile: ";
	errorLocation += file_;
	errorLocation += "\nFunction:";
	errorLocation += function_;
	errorLocation += "\nLine:";
	errorLocation += line_;

	errorMessage += StringHelper::StringToWide(errorLocation);

	MessageBox(NULL, errorMessage.c_str(), L"Error", 0);
}

void ErrorHandler::LogFatalError(HRESULT hr_, const std::string& msg_, const std::string& file_, const std::string& function_, int line_)
{
	_com_error error(hr_);
	std::wstring errorMessage = StringHelper::StringToWide(msg_);
	errorMessage += L"\n";
	errorMessage += error.ErrorMessage();

	std::string errorLocation = "\n\nFile: ";
	errorLocation += file_;
	errorLocation += "\nFunction:";
	errorLocation += function_;
	errorLocation += "\nLine:";
	errorLocation += line_;

	errorMessage += StringHelper::StringToWide(errorLocation);

	MessageBox(NULL, errorMessage.c_str(), L"Fatal Error", 0);
	exit(-1);
}

void ErrorHandler::LogFatalError(const std::string& msg_, const std::string& file_, const std::string& function_, int line_)
{
	std::wstring errorMessage = L"FATAL ERROR";
	errorMessage += L"\n";

	std::string errorLocation = "\n\nFile: ";
	errorLocation += file_;
	errorLocation += "\nFunction:";
	errorLocation += function_;
	errorLocation += "\nLine:";
	errorLocation += line_;

	errorMessage += StringHelper::StringToWide(errorLocation);

	MessageBox(NULL, errorMessage.c_str(), L"Fatal Error", 0);
	exit(-1);
}
