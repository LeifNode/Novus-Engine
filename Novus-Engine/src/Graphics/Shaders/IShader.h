//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_ISHADER_H
#define NOVUS_ISHADER_H

namespace novus
{

class D3DRenderer;

class IShader
{
	virtual void Bind(D3DRenderer* renderer) = 0;
};

}

#endif