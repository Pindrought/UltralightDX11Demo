#pragma once
#include <Ultralight/Listener.h>
#include <Windows.h>
namespace ul = ultralight;

class HtmlViewViewListener : public ul::ViewListener
{
public:
	HtmlViewViewListener();
	void OnChangeTitle(ul::View* caller, const ul::String& title) override;
	void OnChangeURL(ul::View* caller, const ul::String& url) override;
	void OnChangeTooltip(ul::View* caller, const ul::String& tooltip) override;
	void OnChangeCursor(ul::View* caller, ul::Cursor cursor) override;
	void OnAddConsoleMessage(ul::View* caller, ul::MessageSource source, ul::MessageLevel level, const ul::String& message, uint32_t lineNumber, uint32_t columnNumber, const ul::String& sourceId) override;
private:
	HCURSOR cursor_hand;
	HCURSOR cursor_arrow;
	HCURSOR cursor_ibeam;
};