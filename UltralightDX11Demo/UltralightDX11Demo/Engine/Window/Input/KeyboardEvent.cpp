#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent()
	:
	type(Type::Invalid),
	key(0u),
	isAutoRepeat(false)
{
}

KeyboardEvent::KeyboardEvent(UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	wParam = wParam_;
	lParam = lParam_;
	unsigned char keycode = static_cast<unsigned char>(wParam);
	key = keycode;
	switch (uMsg_)
	{
	case WM_KEYDOWN:
		type = KeyboardEvent::Type::KeyDown;
		isAutoRepeat = lParam & 0x40000000;
		break;
	case WM_KEYUP:
		type = KeyboardEvent::Type::KeyUp;
		isAutoRepeat = false;
		break;
	case WM_SYSKEYDOWN:
		type = KeyboardEvent::Type::KeyDown;
		isAutoRepeat = lParam & 0x40000000;
		isSystemKey = true;
		break;
	case WM_SYSKEYUP:
		type = KeyboardEvent::Type::KeyUp;
		isAutoRepeat = false;
		isSystemKey = true;
		break;
	case WM_CHAR:
		type = KeyboardEvent::Type::Char;
		isAutoRepeat = lParam & 0x40000000;
		break;
	default:
		type = KeyboardEvent::Type::Invalid;
		break;
	}
}

KeyboardEvent::Type KeyboardEvent::GetType()
{
	return type;
}

bool KeyboardEvent::IsKeyDown() const
{
	return type == Type::KeyDown;
}

bool KeyboardEvent::IsKeyUp() const
{
	return type == Type::KeyUp;
}

bool KeyboardEvent::IsAutoRepeat() const
{
	return isAutoRepeat;
}

bool KeyboardEvent::IsValid() const
{
	return type != Type::Invalid;
}

bool KeyboardEvent::IsSystemKey() const
{
	return isSystemKey;
}

unsigned char KeyboardEvent::GetKeyCode() const
{
	return key;
}

ul::KeyEvent KeyboardEvent::ToUltralightKeyboardEvent()
{
	return ul::KeyEvent(static_cast<ul::KeyEvent::Type>(type), wParam, lParam, isSystemKey);
}
