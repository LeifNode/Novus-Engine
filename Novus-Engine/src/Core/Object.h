//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_OBJECT_H
#define NOVUS_OBJECT_H

#include "Utils/MetaData/MetaData.h"
#include "Utils/Objects/Type.h"

#define NOVUS_OBJECT_DECL(type)				  \
public:										  \
	static const Type* GetStaticType()		  \
	{										  \
		return novus::MetaSingleton<type>::GetType();\
	}										  \
											  \
	const Type* GetType() const				  \
	{										  \
		return novus::MetaSingleton<type>::GetType();\
	}										  \

#define NOVUS_OBJECT_DEF(type) \
	novus::Type novus::MetaSingleton<type>::mMetaData = novus::Type(#type); \

namespace novus
{
class Object
{
public:
	bool IsA(const Type* t);

	virtual const Type* GetType() const = 0;
};

}

#endif
