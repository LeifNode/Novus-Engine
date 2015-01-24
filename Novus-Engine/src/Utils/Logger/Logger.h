//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_LOGGER_H
#define NOVUS_LOGGER_H

#include <vector>
#include <string>

namespace novus
{

namespace LogLevel
{
	enum Type
	{
		Message,
		Warning,
		Error,
		Critical //Will also make application exit
	};
}

class Logger
{
public:
	struct LogEntry
	{
		std::string message;
		std::string tag;
		LogLevel::Type errorLevel;
		std::string fileName;
		int lineNumber;
	};

	static Logger* getInstance();

	void Log(const char* message, const char* tag, LogLevel::Type errorLevel, const char* fileName, int lineNumber);


private:
	Logger() {}
	~Logger() {}

	void dispatchLogEvent(const LogEntry& entry);

private:
	static Logger* mspInstance;

	std::vector<LogEntry> mMessages;
};

}

#ifndef NE_LOG
#define NE_LOG(message, tag, errorLevel) \
	(Logger::getInstance().Log(message, tag, errorLevel, __FILE__, __LINE__))
#endif

#ifndef NE_MESSAGE
#define NE_MESSAGE(message, tag) \
	(Logger::getInstance().Log(message, tag, novus::LogLevel::Message, __FILE__, __LINE__))
#endif

#ifndef NE_WARN
#define NE_WARN(message, tag) \
	(Logger::getInstance().Log(message, tag, novus::LogLevel::Warning, __FILE__, __LINE__))
#endif

#ifndef NE_ERROR
#define NE_ERROR(message, tag) \
	(Logger::getInstance().Log(message, tag, novus::LogLevel::Error, __FILE__, __LINE__))
#endif

#ifndef NE_CRITICAL
#define NE_CRITICAL(message, tag) \
	(Logger::getInstance().Log(message, tag, novus::LogLevel::Critical, __FILE__, __LINE__))
#endif

#endif