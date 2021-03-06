#include "Utils/GameTimer.h"

namespace novus
{

GameTimer::GameTimer()
	: mSecondsPerCount(0.0), mDeltaTime(0.0), mBaseTime(0),
	mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;

	Reset();
	Tick();
}

float GameTimer::TotalTime()const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
}

double GameTimer::DeltaTime()const
{
	return mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);


	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// Time difference between this frame and the previous.
	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	// Prepare for next frame.
	mPrevTime = mCurrTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

void GameTimer::SleepForDuration(const float& ms)
{
	__int64 currentTime, previousTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	double remainingTime = ms - (double)(currentTime - mPrevTime) * mSecondsPerCount * 1000.0;
	while (remainingTime > 0.0)
	{
		previousTime = currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		double elapsedTime = (double)(currentTime - previousTime) * mSecondsPerCount * 1000.0;

		remainingTime -= elapsedTime;

		/*if (remainingTime > 10.0)
		{
		Sleep(10);
		}*/
	}
}

}//namespace novus