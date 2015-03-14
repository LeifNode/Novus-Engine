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

typedef unsigned int TypeId;

namespace novus
{
class Type
{
public:
	Type(const char* className);
	~Type();

	TypeId GetTypeId() const { return mTypeHash; }
	const char* GetTypeName() const { return mTypeName; }

	bool operator ==(const Type& other) const;
	bool operator !=(const Type& other) const;

private:
	TypeId mTypeHash;
	const char* mTypeName;
};

}

#endif