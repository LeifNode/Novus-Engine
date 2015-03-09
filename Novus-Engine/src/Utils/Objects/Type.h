//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_TYPE_H
#define NOVUS_TYPE_H

#include <string>
#include <stdint.h>

namespace novus
{
class Type
{
public:
	Type(const std::string& className);
	Type(uint32_t classNameHash);
	~Type();

	uint32_t getTypeHash() const { return mTypeHash; }

	bool operator ==(const Type& other);
	bool operator !=(const Type& other);

private:
	uint32_t mTypeHash;
};

}

#endif