#include "MouseEvent.h"

MouseEvent::MouseEvent()
	:
	type(Type::UninitializedType),
	button(Button::UninitializedButton),
	x(0),
	y(0)
{}

MouseEvent::MouseEvent(UINT uMsg_, WPARAM wParam_, LPARAM lParam_)
{
	x = LOWORD(lParam_);
	y = HIWORD(lParam_);
	switch (uMsg_)
	{
	case WM_MOUSEMOVE:
		button = Button::None;
		type = Type::MouseMove;
		break;
	case WM_LBUTTONDOWN:
		button = Button::Left;
		type = Type::MouseDown;
		break;
	case WM_RBUTTONDOWN:
		button = Button::Right;
		type = Type::MouseDown;
		break;
	case WM_MBUTTONDOWN:
		button = Button::Middle;
		type = Type::MouseDown;
		break;
	case WM_LBUTTONUP:
		button = Button::Left;
		type = Type::MouseUp;
		break;
	case WM_RBUTTONUP:
		button = Button::Right;
		type = Type::MouseUp;
		break;
	case WM_MBUTTONUP:
		button = Button::Middle;
		type = Type::MouseUp;
		break;
	default:
		break;
	}
}

bool MouseEvent::IsValid() const
{
	return (type != Type::UninitializedType && button != Button::UninitializedButton);
}

MouseEvent::Type MouseEvent::GetType() const
{
	return type;
}

MouseEvent::Button MouseEvent::GetButton() const
{
	return button;
}

MousePoint MouseEvent::GetPos() const
{
	return { x,y };
}

int MouseEvent::GetPosX() const
{
	return x;
}

int MouseEvent::GetPosY() const
{
	return y;
}

ul::MouseEvent MouseEvent::ToUltralightMouseEvent()
{
	ul::MouseEvent mouseEvent{};
	mouseEvent.button = static_cast<ul::MouseEvent::Button>(button);
	mouseEvent.type = static_cast<ul::MouseEvent::Type>(type);
	mouseEvent.x = x;
	mouseEvent.y = y;
	return mouseEvent;
}
