#include "ConsoleLogSerializer.h"
#include <iostream>

using novus::ConsoleLogSerializer;

void ConsoleLogSerializer::Serialize(const novus::Logger::LogEntry& entry)
{
	std::cout << "[" << entry.tag << "]: " << entry.message << std::endl;
}