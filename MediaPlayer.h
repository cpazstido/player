#ifndef H_MEDIAPLAYER
#define H_MEDIAPLAYER

#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <string>

namespace FB { class PluginWindow; };
struct PlayerContext;
typedef boost::shared_ptr<PlayerContext> PlayerContextPtr;

class MediaPlayer
{
public:
    struct InitializationException : std::runtime_error {
        InitializationException(const char* const what) : std::runtime_error(what) {}
    };

    MediaPlayer();
    ~MediaPlayer();

    const std::string& version() const;
    const std::string& type() const;
    const std::string& lastError() const;

    bool play(const std::string& file);
    bool stop();

    void setWindow(FB::PluginWindow*);

private:
    boost::shared_ptr<PlayerContext> m_context;
    std::string m_version;
    const std::string m_type;
};

#endif

