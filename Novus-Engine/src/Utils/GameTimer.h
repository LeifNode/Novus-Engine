//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//================================================================
//	Implementation of Frank Luna's GameTimer
//================================================================

#pragma once

#ifndef GAME_TIMER_H
#define GAME_TIMER_H

#include "Application/Common.h"

namespace novus
{

class GameTimer
{
public:
	GameTimer();

	///<summary>
	///Returns the total time elapsed since Reset() was called, NOT counting any time when the clock is stopped.
	///</summary>
	///<returns>The total elapsed time in seconds</returns>
	float TotalTime()const;  // in seconds

	///<summary>
	///Returns the time elapsed in between the last two <code>Tick()</code> calls.
	///</summary>
	///<returns>The time in between ticks in seconds</returns>
	double DeltaTime()const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.
	void SleepForDuration(const float&);

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

}

#endif