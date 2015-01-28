//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_ILOG_SERIALIZER_H
#define NOVUS_ILOG_SERIALIZER_H

#include "Logger.h"

namespace novus
{
class ILogSerializer
{
public:
	virtual void Serialize(const Logger::LogEntry& logEntry) = 0;
	//virtual void SerializeAll(const std::vector<Logger::LogEntry>& logHistory);
};
}

#endif