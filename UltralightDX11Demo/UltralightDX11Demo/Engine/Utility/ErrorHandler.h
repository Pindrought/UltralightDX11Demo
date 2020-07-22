#pragma once
#include "../../pch.h"

class ErrorHandler
{
public:
	static void LogCriticalError(const std::string& msg);
	static void LogCriticalError(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line);
	static void LogFatalError(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line);
	static void LogFatalError(const std::string& msg, const std::string& file, const std::string& function, int line);
};

#define ReturnFalseIfFail( hr, msg ) if( FAILED( hr ) ) { ErrorHandler::LogCriticalError( hr, msg, __FILE__, __FUNCTION__, __LINE__ ); return false; }
#define FatalErrorIfFail( hr, msg ) if( FAILED( hr ) ) { ErrorHandler::LogFatalError( hr, msg, __FILE__, __FUNCTION__, __LINE__ ); }
#define FatalError(msg ) ErrorHandler::LogFatalError(msg, __FILE__, __FUNCTION__, __LINE__ ); 
