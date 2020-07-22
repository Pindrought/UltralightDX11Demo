#include "pch.h"
#include "Engine/Engine.h"

int WINAPI wWinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance_, PWSTR pCmdLine_, int nCmdShow_)
{
	Engine engine;
	if (engine.Initialize(1600, 900, "Window Title"))
	{
		Timer t;
		t.Start();
		while (engine.IsRunning())
		{
			const float deltaTime = t.GetMilisecondsElapsed();
			t.Restart();
			engine.Tick(deltaTime);
			engine.Render();
		}
	}
	return 0;
}