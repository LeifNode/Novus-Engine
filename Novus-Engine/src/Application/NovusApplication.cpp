#include "Application/NovusApplication.h"
#include "Graphics/D3DRenderer.h"
#include "EngineStatics.h"
#include "Utils/Memory/MallocTracker.h"

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

using novus::NovusApplication;

novus::NovusApplication* gpApplication = NULL;

namespace
{
	NovusApplication* gNovusApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gNovusApp->MsgProc(hwnd, msg, wParam, lParam);
}

NovusApplication::NovusApplication(HINSTANCE hInstance)
	:
	mhAppInstance(hInstance),
	mMainWndCaption(L"Novus Application"),
	mClientWidth(1980),
	mClientHeight(1080),
	mhWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	mRunning(true)
{
	gNovusApp = this;
	gpApplication = this;

	mpRenderer = NE_NEW D3DRenderer();


	EngineStatics::mspApplication = this;
	EngineStatics::mspRenderer = mpRenderer;
}

NovusApplication::~NovusApplication()
{
	NE_DELETE(mpRenderer);

	novus::MallocTracker::getInstance()->DumpTrackedMemory();
}

int NovusApplication::getClientWidth() const
{
	return mClientWidth;
}

int NovusApplication::getClientHeight() const
{
	return mClientHeight;
}

float NovusApplication::getAspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

HWND NovusApplication::getMainWnd() const
{
	return mhWnd;
}

HWND NovusApplication::getConsoleWnd() const
{
	return mhConsoleWnd;
}

int NovusApplication::Run()
{
	MSG msg = { 0 };

	mTimer.Reset();

	while (msg.message != WM_QUIT && mRunning)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
				//EventSystem::get()->update();
				//LeapManager::getInstance().Update(0.0f);

				if (!mRunning)
					break;

				Update((float)mTimer.DeltaTime());

				if (!mRunning)
					break;

				Render();
				//mTimer.SleepForDuration(16.66f);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

void NovusApplication::Exit()
{
	mRunning = false;
}

bool NovusApplication::Init()
{
	if (!InitConsole())
		return false;

	if (!InitWindow())
		return false;

	if (!mpRenderer->Init())
		return false;

	//LeapManager::getInstance().Initialize();
	
	//mpFontManager->Initialize();

	//LineRenderer::Initialize();

	//mpPhysicsSystem->Initialize();

	return true;
}

void NovusApplication::OnResize()
{
	mpRenderer->OnResize();
}

LRESULT NovusApplication::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (mpRenderer->device())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	//default:
		//if (mpInputSystem->processWindowsMessage(hwnd, msg, wParam, lParam))
			//return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool NovusApplication::InitWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	DWORD wsStyle = WS_POPUP | WS_OVERLAPPEDWINDOW;

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, wsStyle, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhWnd = CreateWindowW(L"D3DWndClassName", mMainWndCaption.c_str(),
		wsStyle | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width / 2, height / 2, NULL, NULL, mhAppInstance, NULL);
	if (!mhWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhWnd, SW_SHOW);
	UpdateWindow(mhWnd);

	RECT rect;

	GetWindowRect(mhWnd, &rect);

	MoveWindow(mhConsoleWnd, rect.right, rect.top, 450, rect.bottom - rect.top, true);

	return true;
}

bool NovusApplication::InitConsole()
{
	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	mhConsoleWnd = GetConsoleWindow();

	if (!mhConsoleWnd)
	{
		MessageBox(0, L"Console Initialize Failed", 0, 0);
		return false;
	}

	return true;
}

void NovusApplication::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhWnd, outs.str().c_str());
		mMainWndCaptionFull = outs.str();

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}