#pragma once
#include "../../../pch.h"
#include "MousePoint.h"

class MouseEvent
{
	friend class Mouse;
public:
	enum class Type
	{
		MouseMove = ul::MouseEvent::Type::kType_MouseMoved,
		MouseDown = ul::MouseEvent::Type::kType_MouseDown,
		MouseUp = ul::MouseEvent::Type::kType_MouseUp,
		MouseMoveRaw,
		UninitializedType
	};
	enum class Button
	{
		None = 0,
		Left = ul::MouseEvent::Button::kButton_Left,
		Middle = ul::MouseEvent::Button::kButton_Middle,
		Right = ul::MouseEvent::Button::kButton_Right,
		UninitializedButton
	};
public:
	MouseEvent();
	MouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool IsValid() const;
	Type GetType() const;
	Button GetButton() const;
	MousePoint GetPos() const;
	int GetPosX() const;
	int GetPosY() const;
	ul::MouseEvent ToUltralightMouseEvent();
private:
	Type type = Type::UninitializedType;
	Button button = Button::UninitializedButton;
	int x;
	int y;
};