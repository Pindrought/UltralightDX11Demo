#include <Ultralight/Ultralight.h>

namespace ul = ultralight;

class HtmlViewLoadListener : public ul::LoadListener
{
public:
	HtmlViewLoadListener() {}

	virtual ~HtmlViewLoadListener() {}
	virtual void OnDOMReady(ul::View* view) override;
private:
};