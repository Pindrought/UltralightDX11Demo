#pragma once
#include "../../../pch.h"

class KeyboardEvent
{
public:
	enum class Type
	{
		Invalid = 0,
		KeyDown = ul::KeyEvent::kType_RawKeyDown,
		KeyUp = ul::KeyEvent::kType_KeyUp,
		Char = ul::KeyEvent::kType_Char,
	};

	KeyboardEvent();
	KeyboardEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	Type GetType();
	bool IsKeyDown() const;
	bool IsKeyUp() const;
	bool IsAutoRepeat() const;
	bool IsValid() const;
	bool IsSystemKey() const;
	unsigned char GetKeyCode() const;

	ul::KeyEvent ToUltralightKeyboardEvent();

private:
	WPARAM wParam = NULL;
	LPARAM lParam = NULL;
	Type type = Type::Invalid;
	wchar_t key = 0;
	bool isAutoRepeat = false;
	bool isSystemKey = false;
};