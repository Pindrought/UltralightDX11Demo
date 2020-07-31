#include "Engine.h"

bool Engine::Initialize(uint32_t width_, uint32_t height_, const char* title_)
{
	if (!window.Initialize(width_, height_, title_))
		return false;

	isRunning = true;

	pMouse = &window.GetMouse();
	pKeyboard = &window.GetKeyboard();

	if (!renderer.Initialize(&window))
		return false;

	if (!htmlViewManager.Initialize(renderer.GetD3DPtr()))
	{
		ErrorHandler::LogCriticalError("Failed to initialize html view manager.");
		return false;
	}

	auto tempView = htmlViewManager.CreateView(width_, height_);
	if (tempView == nullptr)
	{
		ErrorHandler::LogCriticalError("Failed to create html view.");
		return false;
	}
	tempView->LoadURL("file:///web/example.html");
	tempView->Focus();

	return true;
}

bool Engine::IsRunning()
{
	return isRunning;
}

void Engine::Tick(float deltaTime_)
{
	if (!window.ProcessMessages())
	{
		isRunning = false;
		return;
	}
	//Process Mouse messages
	while (!pMouse->EventBufferIsEmpty())
	{
		MouseEvent me = pMouse->ReadEvent();
		if (me.IsValid())
		{
			htmlViewManager.FireMouseEvent(me);
		}
	}
	//Process Keyboard messages
	while (!pKeyboard->EventBufferIsEmpty())
	{
		KeyboardEvent kbe = pKeyboard->ReadEvent();
		if (kbe.IsValid())
		{
			htmlViewManager.FireKeyboardEvent(kbe);
		}
	}
}

void Engine::Render()
{
	renderer.GetD3DPtr()->deviceContext->ClearState();
	htmlViewManager.UpdateViews();
	renderer.BeginFrame();
	for (auto& view : htmlViewManager.GetViews())
	{
		renderer.RenderView(view);
	}
	renderer.EndFrame();
}
