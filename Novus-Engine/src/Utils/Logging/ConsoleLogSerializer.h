//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#include "ILogSerializer.h"

#ifndef NOVUS_CONSOLE_LOG_SERIALIZER_H
#define NOVUS_CONSOLE_LOG_SERIALIZER_H

namespace novus
{
class ConsoleLogSerializer : public ILogSerializer
{
public:
	virtual void Serialize(const Logger::LogEntry& entry) override;
};
}

#endif