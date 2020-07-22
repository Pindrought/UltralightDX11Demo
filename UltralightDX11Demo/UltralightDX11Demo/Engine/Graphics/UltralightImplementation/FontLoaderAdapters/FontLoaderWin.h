//Note: I didn't edit any of this code - Jacob Preston

#include <Ultralight/platform/FontLoader.h>
#include <map>

namespace ul = ultralight;

class FontLoaderWin : public ul::FontLoader
{
public:
    FontLoaderWin() {}
    virtual ~FontLoaderWin() {}
    virtual ul::String16 fallback_font() const override;
    virtual ul::String16 fallback_font_for_characters(const ul::String16& characters, int weight, bool italic) const override;
    virtual ul::RefPtr<ul::FontFile> Load(const ul::String16& family, int weight, bool italic) override;
protected:
    std::map<uint32_t, ul::RefPtr<ul::Buffer>> fonts_;
};
