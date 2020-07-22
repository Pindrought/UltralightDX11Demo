#include "Window.h"

bool Window::Initialize(uint16_t width_, uint16_t height_, const char* title_, int xPos_, int yPos_)
{
	//Assign variables based off incoming parms
	width = width_;
	height = height_;
	title = title_;

	int xPos = xPos_;
	int yPos = yPos_;
	//Determine window x/y position
	if (xPos == INT_MAX) //if no xpos entered, set to center of screen
	{
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int halfScreenWidth = screenWidth / 2;
		int halfWindowWidth = width / 2;
		int centerScreenX = halfScreenWidth - halfWindowWidth;
		xPos = centerScreenX;
	}

	if (yPos == INT_MAX) //if no ypos entered, set to center of screen
	{
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		int halfScreenHeight = screenHeight / 2;
		int halfWindowHeight = height / 2;
		int centerScreenY = halfScreenHeight - halfWindowHeight;
		yPos = centerScreenY;
	}
	//Determine Window Rect
	RECT wr; //Window Rectangle
	wr.left = xPos;
	wr.top = yPos;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;

	DWORD windowStyle = WS_POPUP | WS_SYSMENU | WS_CAPTION | WS_SIZEBOX;
	BOOL result = AdjustWindowRect(&wr, windowStyle, FALSE);
	if (result == 0) //If adjustwindowrect fails...
		return false;

	//Register Window Class
	RegisterWindowClass();
	//Create Window
	hwnd = CreateWindowExA(0, //Extended Windows style - we are using the default. For other options, see: https://msdn.microsoft.com/en-us/library/windows/desktop/ff700543(v=vs.85).aspx
		windowClass, //Window class name
		title.c_str(), //Window Title
		windowStyle, //Windows style - See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632600(v=vs.85).aspx
		wr.left, //Window X Position
		wr.top, //Window Y Position
		wr.right - wr.left, //Window Width
		wr.bottom - wr.top, //Window Height
		NULL, //Handle to parent of this window. Since this is the first window, it has no parent window.
		NULL, //Handle to menu or child window identifier. Can be set to NULL and use menu in WindowClassEx if a menu is desired to be used.
		GetModuleHandle(NULL), //Handle to the instance of module to be used with this window
		this); //Parameter passed to create window 'WM_NCCREATE'

	if (hwnd == NULL)
	{
		DWORD error = GetLastError();
		return false;
	}
	//Show/focus Window
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	return true;
}

bool Window::ProcessMessages()
{
	// Handle the windows messages.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG)); // Initialize the message structure.

	while (PeekMessage(&msg, //Where to store message (if one exists) See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644943(v=vs.85).aspx
		hwnd, //Handle to window we are checking messages for
		0,    //Minimum Filter Msg Value - We are not filtering for specific messages, but the min/max could be used to filter only mouse messages for example.
		0,    //Maximum Filter Msg Value
		PM_REMOVE))//Remove message after capturing it via PeekMessage. For more argument options, see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644943(v=vs.85).aspx
	{
		TranslateMessage(&msg); //Translate message from virtual key messages into character messages so we can dispatch the message. See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644955(v=vs.85).aspx
		DispatchMessage(&msg); //Dispatch message to our Window Proc for this window. See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644934(v=vs.85).aspx
	}

	// Check if the window was closed
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(hwnd))
		{
			hwnd = NULL; //Message processing loop takes care of destroying this window
			UnregisterClassA(windowClass, GetModuleHandle(NULL));
			return false;
		}
	}

	return true;
}

LRESULT Window::WindowProcA(HWND hwnd_, UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	switch (uMsg_)
	{
		//Mouse Messages
		case WM_MOUSEMOVE:
			mouse.OnWindowsMouseMessage(uMsg_, wParam_, lParam_);
			return 0;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			mouse.OnWindowsMouseMessage(uMsg_, wParam_, lParam_);
			SetCapture(hwnd);
			return 0;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			mouse.OnWindowsMouseMessage(uMsg_, wParam_, lParam_);
			ReleaseCapture();
			return 0;
		//Keyboard Messages
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_CHAR:
			keyboard.OnWindowsKeyboardMessage(uMsg_, wParam_, lParam_);
			return 0;
		default:
			return DefWindowProcA(hwnd_, uMsg_, wParam_, lParam_);
	}
}

Mouse& Window::GetMouse()
{
	return mouse;
}

Keyboard& Window::GetKeyboard()
{
	return keyboard;
}

HWND Window::GetHWND() const
{
	return hwnd;
}

uint16_t Window::GetWidth() const
{
	return width;
}

uint16_t Window::GetHeight() const
{
	return height;
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd_, UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	switch (uMsg_)
	{
		// All other messages
	case WM_CLOSE:
		DestroyWindow(hwnd_);
		return 0;

	default:
	{
		// retrieve ptr to window class
		Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd_, GWLP_USERDATA));
		// forward message to window class handler
		return pWindow->WindowProcA(hwnd_, uMsg_, wParam_, lParam_);
	}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd_, UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	switch (uMsg_)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam_);
		Window* pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) //Sanity check
		{
			exit(-1);
		}
		SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		return pWindow->WindowProcA(hwnd_, uMsg_, wParam_, lParam_);
	}
	default:
		return DefWindowProcA(hwnd_, uMsg_, wParam_, lParam_);
	}
}

void Window::RegisterWindowClass()
{
	WNDCLASSEXA wc = {}; //Our Window Class (This has to be filled before our window can be created) See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633577(v=vs.85).aspx
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //Flags [Redraw on width/height change from resize/movement] See: https://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
	wc.lpfnWndProc = HandleMessageSetup; //Pointer to Window Proc function for handling messages from this window
	wc.cbClsExtra = 0; //# of extra bytes to allocate following the window-class structure. We are not currently using this.
	wc.cbWndExtra = 0; //# of extra bytes to allocate following the window instance. We are not currently using this.
	wc.hInstance = GetModuleHandle(NULL); //Handle to the instance that contains the Window Procedure
	wc.hIcon = NULL;   //Handle to the class icon. Must be a handle to an icon resource. We are not currently assigning an icon, so this is null.
	wc.hIconSm = NULL; //Handle to small icon for this class. We are not currently assigning an icon, so this is null.
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Default Cursor - If we leave this null, we have to explicitly set the cursor's shape each time it enters the window.
	wc.hbrBackground = NULL; //Handle to the class background brush for the window's background color - we will leave this blank for now and later set this to black. For stock brushes, see: https://msdn.microsoft.com/en-us/library/windows/desktop/dd144925(v=vs.85).aspx
	wc.lpszMenuName = NULL; //Pointer to a null terminated character string for the menu. We are not using a menu yet, so this will be NULL.
	wc.lpszClassName = windowClass; //Pointer to null terminated string of our class name for this window.
	wc.cbSize = sizeof(WNDCLASSEXA); //Need to fill in the size of our struct for cbSize
	RegisterClassExA(&wc); // Register the class so that it is usable.
}
