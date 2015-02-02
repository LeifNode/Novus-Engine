#include "ConsoleLogSerializer.h"
#include <iostream>

namespace novus
{

void ConsoleLogSerializer::Serialize(const Logger::LogEntry& entry)
{
	std::cout << "[" << entry.tag << "]: " << entry.message << std::endl;
}

}