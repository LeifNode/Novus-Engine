#include "Logger.h"
#include "ILogSerializer.h"
#include "Utils/Memory/Memory.h"

namespace novus
{

Logger* Logger::mspInstance = NULL;

Logger* Logger::getInstance()
{
	if (mspInstance == NULL)
		mspInstance = NE_NEW Logger();

	return mspInstance;
}

Logger::~Logger()
{
	
}

void Logger::Log(const char* message, const char* tag, LogLevel::Type errorLevel, const char* fileName, int lineNumber)
{
	LogEntry entry;
	entry.message = message;
	entry.tag = tag;
	entry.errorLevel = errorLevel;
	entry.fileName = fileName;
	entry.lineNumber = lineNumber;

	mMessages.push_back(entry);
	DispatchLogEvent(entry);
}

void Logger::AddSerializer(ILogSerializer* serializer, LogLevel::Type levelMask, const char* tagMask)
{
	std::string tagString = tagMask;

	for (auto listIt = mSerializers.begin(); listIt != mSerializers.end(); ++listIt)
	{
		if ((*listIt).serializer == serializer &&
			(*listIt).levelMask == levelMask &&
			(*listIt).tagMask == tagString)
			return;
	}

	SerializerEntry entry;
	entry.serializer = serializer;
	entry.levelMask = levelMask;
	entry.tagMask = tagString;

	mSerializers.push_back(entry);
}

void Logger::RemoveSerializer(ILogSerializer* serializer)
{
	mSerializers.remove_if(
		[serializer](SerializerEntry entry)
	{
		return entry.serializer == serializer;
	});
}

void Logger::RemoveSerializer(ILogSerializer* serializer, LogLevel::Type levelMask, const char* tagMask)
{
	std::string tagString = tagMask;

	mSerializers.remove_if(
		[serializer, levelMask, &tagString](SerializerEntry entry)
	{
		return entry.serializer == serializer &&
			entry.levelMask == levelMask &&
			entry.tagMask == tagString;
	});
}

void Logger::DispatchLogEvent(const LogEntry& entry)
{
	for (auto it = mSerializers.begin(); it != mSerializers.end(); ++it)
	{
		//if (((*it).levelMask & entry.errorLevel >= 1) && ((*it).tagMask == std::string(entry.tag)))
		(*it).serializer->Serialize(entry);
	}
}

}