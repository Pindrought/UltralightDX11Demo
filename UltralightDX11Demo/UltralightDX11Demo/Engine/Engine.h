#pragma once
#include "../pch.h"
#include "Window/Window.h"
#include "Graphics/Renderer.h"
#include "Graphics/UltralightImplementation/HtmlViewManager.h"

class Engine
{
public:
	bool Initialize(uint32_t width, uint32_t height, const char* title);
	bool IsRunning();
	void Tick(float deltaTime);
	void Render();
private:
	Window window;
	Renderer renderer;
	Mouse* pMouse = nullptr;
	Keyboard* pKeyboard = nullptr;
	HtmlViewManager htmlViewManager;
	bool isRunning = false;
};