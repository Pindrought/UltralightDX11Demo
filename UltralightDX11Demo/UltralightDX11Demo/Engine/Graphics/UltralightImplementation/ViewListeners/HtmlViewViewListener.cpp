#include "HtmlViewViewListener.h"
#include <Windows.h>

HtmlViewViewListener::HtmlViewViewListener()
{
    cursor_hand = LoadCursor(NULL, IDC_HAND);
    cursor_arrow = LoadCursor(NULL, IDC_ARROW);
    cursor_ibeam = LoadCursor(NULL, IDC_IBEAM);
}

void HtmlViewViewListener::OnChangeTitle(ul::View* caller_, const ul::String& title_)
{
}

void HtmlViewViewListener::OnChangeURL(ul::View* caller_, const ul::String& url_)
{
}

void HtmlViewViewListener::OnChangeTooltip(ul::View* caller_, const ul::String& tooltip_)
{
}

void HtmlViewViewListener::OnChangeCursor(ul::View* caller_, ul::Cursor cursor_)
{
    switch (cursor_)
    {
    case ultralight::kCursor_Hand:
    {
        SetCursor(cursor_hand);
        break;
    }
    case ultralight::kCursor_Pointer:
    {
        SetCursor(cursor_arrow);
        break;
    }
    case ultralight::kCursor_IBeam:
    {
        SetCursor(cursor_ibeam);
        break;
    }
    }
}

void HtmlViewViewListener::OnAddConsoleMessage(ul::View* caller_, ul::MessageSource source_, ul::MessageLevel level_, const ul::String& message_, uint32_t lineNumber_, uint32_t columnNumber_, const ul::String& sourceId_)
{
    OutputDebugStringA(message_.utf8().data());
    OutputDebugStringA("\n");
}
