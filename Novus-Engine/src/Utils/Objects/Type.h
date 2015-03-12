//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_TYPE_H
#define NOVUS_TYPE_H

#include <stdint.h>

namespace novus
{
class Type
{
public:
	Type(const char* className);
	~Type();

	uint32_t getTypeHash() const { return mTypeHash; }
	const char* getTypeName() const { return mTypeName; }

	bool operator ==(const Type& other) const;
	bool operator !=(const Type& other) const;

private:
	uint32_t mTypeHash;
	const char* mTypeName;
};

}

#endif