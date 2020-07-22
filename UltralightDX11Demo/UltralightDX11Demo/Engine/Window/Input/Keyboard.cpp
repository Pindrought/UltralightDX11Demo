#include "Keyboard.h"

Keyboard::Keyboard()
{
}

bool Keyboard::KeyIsPressed(const unsigned char keycode_)
{
	return keyStates[keycode_];
}

bool Keyboard::EventBufferIsEmpty()
{
	return eventBuffer.empty();
}

KeyboardEvent Keyboard::ReadEvent()
{
	if (eventBuffer.empty()) //If no keys to be read?
	{
		return KeyboardEvent(); //return empty keyboard event
	}
	else
	{
		KeyboardEvent e = eventBuffer.front(); //Get first Keyboard Event from queue
		eventBuffer.pop(); //Remove first item from queue
		return e; //Returns keyboard event
	}
}

void Keyboard::OnWindowsKeyboardMessage(UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	KeyboardEvent kbe(uMsg_, wParam_, lParam_);
	switch (kbe.GetType())
	{
	case KeyboardEvent::Type::KeyDown:
		keyStates[kbe.GetKeyCode()] = true;
		break;
	case KeyboardEvent::Type::KeyUp:
		keyStates[kbe.GetKeyCode()] = false;
		break;
	case KeyboardEvent::Type::Char:
		break;
	}
	eventBuffer.push(kbe);
}
