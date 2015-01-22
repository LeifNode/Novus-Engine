#include "Logger.h"

using novus::Logger;

Logger* Logger::mspInstance = NULL;

Logger* Logger::getInstance()
{
	if (mspInstance == NULL)
		mspInstance = new Logger();

	return mspInstance;
}

void Logger::Log(const char* message, const char* tag, novus::LogLevel::Type errorLevel, const char* fileName, int lineNumber)
{
	LogEntry entry;
	entry.message = message;
	entry.tag = tag;
	entry.errorLevel = errorLevel;
	entry.fileName = fileName;
	entry.lineNumber = lineNumber;

	mMessages.push_back(entry);
}