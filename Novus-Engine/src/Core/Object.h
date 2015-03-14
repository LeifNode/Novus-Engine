//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_OBJECT_H
#define NOVUS_OBJECT_H

namespace novus
{

class Class;

class Object
{
public:
	bool IsA(const Class& t);

	virtual Class* GetStaticClass() const = 0;
};

}

#endif
