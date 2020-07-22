#include "Mouse.h"

int Mouse::GetPosX()
{
	return x;
}

int Mouse::GetPosY()
{
	return y;
}

MousePoint Mouse::GetPos()
{
	return { x,y };
}

bool Mouse::EventBufferIsEmpty()
{
	return eventBuffer.empty();
}

MouseEvent Mouse::ReadEvent()
{
	if (eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = eventBuffer.front(); //Get first event from buffer
		eventBuffer.pop(); //Remove first event from buffer
		return e;
	}
}

void Mouse::OnWindowsMouseMessage(UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	MouseEvent mouseEvent(uMsg_, wParam_, lParam_);
	switch (mouseEvent.type)
	{
	case MouseEvent::Type::MouseDown:
		lastPressedButton = mouseEvent.button;
		break;
	case MouseEvent::Type::MouseUp:
		lastPressedButton = MouseEvent::Button::None;
		break;
	case MouseEvent::Type::MouseMove:
		mouseEvent.button = lastPressedButton;
		break;
	}
	eventBuffer.push(mouseEvent);
}
