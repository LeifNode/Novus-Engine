//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_META_DATA_H
#define NOVUS_META_DATA_H

#include <string>
#include "Utils/Objects/Type.h"

namespace novus
{

template <typename T>
class MetaSingleton
{
public:
	static const Type* GetType() { return &mMetaData; }

private:
	static Type mMetaData;
};

template <typename T>
class MetaSingleton<T*> : public MetaSingleton<T>
{};

template <typename T>
class MetaSingleton<const T> : public MetaSingleton<T>
{};

template <typename T>
class MetaSingleton<const T*> : public MetaSingleton<T>
{};

template <typename T>
class MetaSingleton<T&> : public MetaSingleton<T>
{};

template <typename T>
class MetaSingleton<const T&> : public MetaSingleton<T>
{};

}

#endif