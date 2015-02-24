//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_OBJ_LOADER_H
#define NOVUS_OBJ_LOADER_H

#include "IModelLoader.h"

namespace novus
{

class OBJLoader : public IModelLoader
{
public:
	OBJLoader();

	~OBJLoader() override;

	bool Load(const std::wstring& path) override;

	assettypes::Scene* getScene() const override;

private:
	OBJLoader(const OBJLoader& other) {}
	OBJLoader& operator= (const OBJLoader& other) { return *this; }

private:
	std::istream* mpFile;

	assettypes::Scene* mpScene;
};

}

#endif
