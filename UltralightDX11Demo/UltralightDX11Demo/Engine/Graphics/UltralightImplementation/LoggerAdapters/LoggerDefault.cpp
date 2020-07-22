#include "LoggerDefault.h"
#include "../../../Utility/ErrorHandler.h"

void LoggerDefault::LogMessage(ul::LogLevel logLevel_, const ul::String16& message_)
{
	if (logLevel_ == ul::LogLevel::kLogLevel_Error || logLevel_ == ul::LogLevel::kLogLevel_Warning)
		ErrorHandler::LogCriticalError(ul::String(message_).utf8().data());
}
