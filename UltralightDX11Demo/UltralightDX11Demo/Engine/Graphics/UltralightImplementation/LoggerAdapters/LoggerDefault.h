#pragma once
#include <Ultralight/platform/Logger.h>

namespace ul = ultralight;
class LoggerDefault : public ul::Logger
{
public:
    void LogMessage(ul::LogLevel logLevel, const ul::String16& message) override;
};