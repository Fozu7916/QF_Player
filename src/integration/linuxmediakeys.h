#ifndef LINUXMEDIAKEYS_H
#define LINUXMEDIAKEYS_H

#include <QObject>
#include <QTimer>
#include <QSocketNotifier>

#ifdef __linux__
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#endif

class LinuxMediaKeys : public QObject
{
    Q_OBJECT
public:
    enum MediaKey {
        PlayPause,
        Next,
        Previous,
        Stop
    };
    
    explicit LinuxMediaKeys(QObject *parent = nullptr);
    ~LinuxMediaKeys();
    
    bool initialize();
    void cleanup();
#ifdef __linux__
    bool isInitialized() const { return m_initialized; }
#else
    bool isInitialized() const { return false; }
#endif

signals:
    void mediaKeyPressed(MediaKey key);
    void playPausePressed();
    void nextPressed();
    void prevPressed();
    void stopPressed();
    
private slots:
    void checkForEvents();
    
private:
#ifdef __linux__
    Display* m_display;
    Window m_rootWindow;
    QSocketNotifier* m_notifier;
    bool m_initialized;
    int m_x11Fd;
    
    // X11 методы
    bool setupX11();
    void cleanupX11();
    void grabMediaKeys();
    void ungrabMediaKeys();
    KeyCode getKeyCode(const char* keyName);
    bool isKeyGrabbed(KeyCode keycode);
    
    // Wayland методы (будущая реализация)
    bool setupWayland();
    void cleanupWayland();
    
    // Общие методы
    void handleKeyPress(KeyCode keycode);
    bool detectDisplayServer();
#endif
};

#endif // LINUXMEDIAKEYS_H
