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
#include <list>
#include <string>

namespace novus
{

class ILogSerializer;

namespace LogLevel
{
	enum Type
	{
		Message = 1,
		Warning = 2,
		Error   = 4,
		Critical = 8 //Will also make application exit
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

	void AddSerializer(ILogSerializer* serializer, LogLevel::Type levelMask, const char* tagMask = "");
	void RemoveSerializer(ILogSerializer* serializer);
	void RemoveSerializer(ILogSerializer* serializer, LogLevel::Type levelMask, const char* tagMask = "");

private:
	struct SerializerEntry
	{
		ILogSerializer* serializer;
		LogLevel::Type  levelMask;
		std::string     tagMask;
	};

private:
	Logger() {}
	~Logger() {}
	Logger(const Logger&);

	Logger& operator= (const Logger&);

	void DispatchLogEvent(const LogEntry& entry);

private:
	static Logger* mspInstance;

	std::vector<LogEntry> mMessages;
	std::list<SerializerEntry> mSerializers;
};

}

#ifndef NE_LOG
#define NE_LOG(message, tag, errorLevel) \
	(novus::Logger::getInstance()->Log(message, tag, errorLevel, __FILE__, __LINE__))
#endif

#ifndef NE_MESSAGE
#define NE_MESSAGE(message, tag) \
	(novus::Logger::getInstance()->Log(message, tag, novus::LogLevel::Message, __FILE__, __LINE__))
#endif

#ifndef NE_WARN
#define NE_WARN(message, tag) \
	(novus::Logger::getInstance()->Log(message, tag, novus::LogLevel::Warning, __FILE__, __LINE__))
#endif

#ifndef NE_ERROR
#define NE_ERROR(message, tag) \
	(novus::Logger::getInstance()->Log(message, tag, novus::LogLevel::Error, __FILE__, __LINE__))
#endif

#ifndef NE_CRITICAL
#define NE_CRITICAL(message, tag) \
	(novus::Logger::getInstance()->Log(message, tag, novus::LogLevel::Critical, __FILE__, __LINE__))
#endif

#endif