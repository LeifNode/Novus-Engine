//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_APPLICATION_H
#define NOVUS_APPLICATION_H

#include "Common.h"
#include "Utils/GameTimer.h"

namespace novus
{

class NovusApplication
{
public:
	NovusApplication(HINSTANCE hInstance);
	virtual ~NovusApplication();

	int Run();

	void Exit();

	virtual bool Init();
	virtual void OnResize();
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	int getClientWidth() const;
	int getClientHeight() const;
	float getAspectRatio() const;

protected:
	bool InitWindow();
	bool InitConsole();

	void CalculateFrameStats();

private:
	HINSTANCE mhAppInstance;
	HWND mhWnd;
	HWND mhConsoleWnd;

	bool mAppPaused, mMinimized, mMaximized, mResizing, mRunning;

	int mClientWidth;
	int mClientHeight;

	std::wstring mMainWndCaptionFull;
	std::wstring mMainWndCaption;

	GameTimer mTimer;
};

}
#endif