#pragma once
#include "../../pch.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"

class Window
{
public:
	bool Initialize(uint16_t width = 800, uint16_t height = 600, const char* title = "Window Title",
					int xPos = INT_MAX, int yPos = INT_MAX);

	bool ProcessMessages();
	LRESULT WindowProcA(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	Mouse& GetMouse();
	Keyboard& GetKeyboard();
	HWND GetHWND() const;
	uint16_t GetWidth() const;
	uint16_t GetHeight() const;
private:
	void RegisterWindowClass();
	Mouse mouse;
	Keyboard keyboard;
	HWND hwnd = NULL; //Window Handle
	std::string title = "";
	const char* windowClass = "EngineWindowClass";
	uint16_t width = 0;
	uint16_t height = 0;
};