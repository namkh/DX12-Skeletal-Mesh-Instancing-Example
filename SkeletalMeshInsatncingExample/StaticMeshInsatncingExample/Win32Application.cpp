#include "GlobalTimer.h"
#include "CoreEventManager.h"
#include "ExampleAppBase.h"

#include "Win32Application.h"
#include "GlobalSystemValues.h"
#include "DX12DeviceResourceAccess.h"

#include "Utils.h"

#include <shellapi.h>
#include <stdio.h>

HWND Win32Application::m_hwnd = nullptr;

static bool g_appMinimized = false;
static bool g_appMaximized = false;
static bool g_appPause = false;
static bool g_appResizing = false;

static uint32_t g_appWidth = 800;
static uint32_t g_appHeight = 600;

static bool g_appStarted = false;

static int g_prevMousePosX = 0;
static int g_prevMousePosY = 0;

bool g_mouseRButtonPressed = false;
bool g_mouseLButtonPressed = false;

int Win32Application::Run(ExampleAppBase* pExample, HINSTANCE hInstance, int nCmdShow)
{
    // Parse the command line parameters
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    LocalFree(argv);

    // Initialize the window class.
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"instancing example";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(pExample->GetWidth()), static_cast<LONG>(pExample->GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window and store a handle to it.
    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        pExample->GetAppName().c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        pExample);
    
    ShowWindow(m_hwnd, nCmdShow);

	g_appPause = false;

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	g_prevMousePosX = cursorPos.x;
	g_prevMousePosY = cursorPos.y;

	g_appWidth = pExample->GetWidth();
	g_appHeight = pExample->GetHeight();

	if (!DX12DeviceResourceAccess::Instance().Initialize(hInstance, m_hwnd, g_appWidth, g_appHeight))
	{
		REPORT_WITH_SHUTDOWN(EReportType::REPORT_TYPE_ERROR, "Device resource create failed.");
	}
	
	GlobalTimer::Instance().Initialize();
	
	if (!pExample->Initialize())
	{
		REPORT_WITH_SHUTDOWN(EReportType::REPORT_TYPE_ERROR, "Example app create failed.");
	}
	g_appStarted = true;
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // Process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            GlobalTimer::Instance().Tick();
			if (!g_appPause)
			{
				gDX12DeviceRes->RefreshDeviceResIfDirtied();

				float deltaTime = GlobalTimer::Instance().GetDeltaTime();
				float fps = 1.0f / deltaTime;
				static wchar_t titleBuffer[1024];
				memset(titleBuffer, 0, sizeof(wchar_t) * 1024);
				_stprintf_s(titleBuffer, L"%s [FPS : %0.1f]", pExample->GetAppName().c_str(), fps);
				SetWindowText(m_hwnd, titleBuffer);

				pExample->Update(deltaTime);
				pExample->PreRender();
				pExample->Render();
			}
        }
    }

	pExample->Destroy();
	DX12DeviceResourceAccess::Instance().Destroy();
    // Return this part of the WM_QUIT message to Windows.
    return static_cast<char>(msg.wParam);
}

// Main message handler for the sample.
LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MouseEvent mouseEvent;
    switch (message)
    {
	case WA_ACTIVE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			g_appPause = true;
		}
		else
		{
			g_appPause = false;
		}
	}
	break;
	case WM_KEYUP:
	{
		{
			KeyboardEvent keyboardChangedEvent;
			keyboardChangedEvent.m_keyType = static_cast<EKeyboaradEvent>(wParam);
			CoreEventManager::Instance().ExecKeyboardEvent(&keyboardChangedEvent);
		}
		break;
	}
	break;
	case WM_SIZE:
		if (g_appStarted)
		{
			g_appWidth = LOWORD(lParam);
			g_appHeight = HIWORD(lParam);
			if (wParam == SIZE_MINIMIZED)
			{
				g_appPause = true;
				g_appMinimized = true;
				g_appMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				if (!g_appMaximized)
				{
					ScreenSizeChangedEvent screenSizeChangedEvent(g_appWidth, g_appHeight);
					CoreEventManager::Instance().ExecScreenSizeChangedEvent(&screenSizeChangedEvent);

					g_appMinimized = false;
					g_appMaximized = true;
				}
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (g_appMinimized)
				{
					ScreenSizeChangedEvent screenSizeChangedEvent(g_appWidth, g_appHeight);
					CoreEventManager::Instance().ExecScreenSizeChangedEvent(&screenSizeChangedEvent);
					g_appPause = false;
					g_appMinimized = false;
				}
				else if (g_appMaximized)
				{
					ScreenSizeChangedEvent screenSizeChangedEvent(g_appWidth, g_appHeight);
					CoreEventManager::Instance().ExecScreenSizeChangedEvent(&screenSizeChangedEvent);
					g_appPause = false;
					g_appMaximized = false;
				}
			}
		}
		break;
	case WM_ENTERSIZEMOVE:
	{
		g_appPause = true;
		g_appResizing = true;
	}
	break;
	case WM_EXITSIZEMOVE:
	{
		g_appPause = false;
		g_appResizing = false;

		ScreenSizeChangedEvent screenSizeChangedEvent(g_appWidth, g_appHeight);
		CoreEventManager::Instance().ExecScreenSizeChangedEvent(&screenSizeChangedEvent);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		short wheel = (short)HIWORD(wParam);
		if (wheel > 0)
		{
			mouseEvent.AddEvent(EMouseEvent::MOUSE_WHEEL_UP);
		}
		else
		{
			mouseEvent.AddEvent(EMouseEvent::MOUSE_WHEEL_DOWN);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		g_mouseRButtonPressed = true;
	}
	break;

	case WM_RBUTTONUP:
	{
		g_mouseRButtonPressed = false;
	}
	break;
	case WM_LBUTTONDOWN:
	{
		g_mouseLButtonPressed = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		g_mouseLButtonPressed = false;
		mouseEvent.AddEvent(EMouseEvent::LBUTTON_UP);
	}
	break;

	case WM_MOUSEMOVE:
	{
		int curMousePosX = LOWORD(lParam);
		int curMousePosY = HIWORD(lParam);
		mouseEvent.m_posX = curMousePosX;
		mouseEvent.m_posY = curMousePosY;
		if (g_mouseRButtonPressed)
		{
			mouseEvent.m_dx = curMousePosX - g_prevMousePosX;
			mouseEvent.m_dy = curMousePosY - g_prevMousePosY;
			mouseEvent.AddEvent(EMouseEvent::MOUSE_MOVE);
		}
		g_prevMousePosX = curMousePosX;
		g_prevMousePosY = curMousePosY;
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
    }

	if (mouseEvent.HasMouseEvent())
	{
		CoreEventManager::Instance().ExecMouseEvent(&mouseEvent);
	}

    // Handle any messages the switch statement didn't.
    return DefWindowProc(hWnd, message, wParam, lParam);
}
