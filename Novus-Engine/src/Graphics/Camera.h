//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CAMERA_H
#define NOVUS_CAMERA_H

#include "Application/Common.h"
#include "Math/Math.h"
#include "Events/Events.h"

///TODO: Factor out movment and rotation controls into a separate class
namespace novus
{
class Camera
{
public:
	Camera();
	~Camera();

	void BindInput();
	void UnbindInput();

	Matrix4 getView() const;
	Matrix4 getProj() const { return mProjection; }

	Vector3 getPosition() const { return mPosition; }
	void setPosition(const Vector3& p);

	float getVelocity() const { return mVelocity; }

	float getNear() const { return mNear; }
	float getFar() const { return mFar; }

	void OnResize(int width, int height);

	void Update(float dt);

	void OnMouseDown(IEventDataPtr eventData);
	void OnMouseUp(IEventDataPtr eventData);
	void OnMouseMove(IEventDataPtr eventData);

private:
	void UpdatePosition(float dt);

private:
	Matrix4 mProjection;

	Vector3 mPosition;
	Vector3 mDirection;

	float mNear;
	float mFar;
	float mVelocity;

	Vector3_t<int> mLastMousePosition;

	bool mReadingMouse;
	bool mFreeRoam;
};
}


#endif