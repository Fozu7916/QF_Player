#include "linuxmediakeys.h"
#include <QDebug>
#include <QApplication>
#include <QLoggingCategory>

#ifdef __linux__
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <fcntl.h>
#endif

Q_LOGGING_CATEGORY(linuxMediaKeys, "app.linuxmediakeys")

LinuxMediaKeys::LinuxMediaKeys(QObject *parent)
    : QObject(parent)
#ifdef __linux__
    , m_display(nullptr)
    , m_rootWindow(0)
    , m_notifier(nullptr)
    , m_initialized(false)
    , m_x11Fd(-1)
#endif
{
}

LinuxMediaKeys::~LinuxMediaKeys() {
    cleanup();
}

bool LinuxMediaKeys::initialize() {
#ifdef __linux__
    if (m_initialized) return true;
    
    qCDebug(linuxMediaKeys) << "Инициализация Linux медиа-клавиш...";
    
    // Определяем тип дисплей-сервера
    if (!detectDisplayServer()) {
        qCWarning(linuxMediaKeys) << "Не удалось определить дисплей-сервер";
        return false;
    }
    
    // Пробуем инициализировать X11
    if (setupX11()) {
        qCDebug(linuxMediaKeys) << "X11 инициализация успешна";
        m_initialized = true;
        return true;
    }
    
    // Пробуем инициализировать Wayland (будущая реализация)
    if (setupWayland()) {
        qCDebug(linuxMediaKeys) << "Wayland инициализация успешна";
        m_initialized = true;
        return true;
    }
    
    qCWarning(linuxMediaKeys) << "Не удалось инициализировать ни X11, ни Wayland";
    return false;
#else
    qCDebug(linuxMediaKeys) << "Linux медиа-клавиши не поддерживаются на этой платформе";
    return false;
#endif
}

void LinuxMediaKeys::cleanup() {
#ifdef __linux__
    if (!m_initialized) return;
    
    qCDebug(linuxMediaKeys) << "Очистка Linux медиа-клавиш...";
    
    cleanupX11();
    cleanupWayland();
    
    if (m_notifier) {
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    
    m_initialized = false;
    qCDebug(linuxMediaKeys) << "Очистка завершена";
#endif
}

void LinuxMediaKeys::checkForEvents() {
#ifdef __linux__
    if (!m_display || !m_initialized) return;
    
    XEvent event;
    while (XPending(m_display)) {
        XNextEvent(m_display, &event);
        
        if (event.type == KeyPress) {
            KeyCode keycode = event.xkey.keycode;
            handleKeyPress(keycode);
        }
    }
#endif
}

#ifdef __linux__
void LinuxMediaKeys::grabMediaKeys() {
    if (!m_display || !m_rootWindow) return;
    
    qCDebug(linuxMediaKeys) << "Захват медиа-клавиш...";
    
    // Получаем коды медиа-клавиш
    KeyCode playPauseKey = getKeyCode("XF86AudioPlay");
    KeyCode nextKey = getKeyCode("XF86AudioNext");
    KeyCode prevKey = getKeyCode("XF86AudioPrev");
    KeyCode stopKey = getKeyCode("XF86AudioStop");
    
    int grabbedCount = 0;
    
    // Захватываем клавиши с проверкой ошибок
    if (playPauseKey) {
        int result = XGrabKey(m_display, playPauseKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) {
            grabbedCount++;
            qCDebug(linuxMediaKeys) << "Play/Pause клавиша захвачена";
        } else {
            qCWarning(linuxMediaKeys) << "Не удалось захватить Play/Pause клавишу";
        }
    }
    
    if (nextKey) {
        int result = XGrabKey(m_display, nextKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) {
            grabbedCount++;
            qCDebug(linuxMediaKeys) << "Next клавиша захвачена";
        } else {
            qCWarning(linuxMediaKeys) << "Не удалось захватить Next клавишу";
        }
    }
    
    if (prevKey) {
        int result = XGrabKey(m_display, prevKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) {
            grabbedCount++;
            qCDebug(linuxMediaKeys) << "Previous клавиша захвачена";
        } else {
            qCWarning(linuxMediaKeys) << "Не удалось захватить Previous клавишу";
        }
    }
    
    if (stopKey) {
        int result = XGrabKey(m_display, stopKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) {
            grabbedCount++;
            qCDebug(linuxMediaKeys) << "Stop клавиша захвачена";
        } else {
            qCWarning(linuxMediaKeys) << "Не удалось захватить Stop клавишу";
        }
    }
    
    XFlush(m_display);
    qCDebug(linuxMediaKeys) << "Захвачено клавиш:" << grabbedCount << "из 4";
}

void LinuxMediaKeys::ungrabMediaKeys() {
    if (!m_display || !m_rootWindow) return;
    
    qCDebug(linuxMediaKeys) << "Освобождение медиа-клавиш...";
    
    KeyCode playPauseKey = getKeyCode("XF86AudioPlay");
    KeyCode nextKey = getKeyCode("XF86AudioNext");
    KeyCode prevKey = getKeyCode("XF86AudioPrev");
    KeyCode stopKey = getKeyCode("XF86AudioStop");
    
    if (playPauseKey) {
        XUngrabKey(m_display, playPauseKey, AnyModifier, m_rootWindow);
    }
    if (nextKey) {
        XUngrabKey(m_display, nextKey, AnyModifier, m_rootWindow);
    }
    if (prevKey) {
        XUngrabKey(m_display, prevKey, AnyModifier, m_rootWindow);
    }
    if (stopKey) {
        XUngrabKey(m_display, stopKey, AnyModifier, m_rootWindow);
    }
    
    XFlush(m_display);
    qCDebug(linuxMediaKeys) << "Медиа-клавиши освобождены";
}

KeyCode LinuxMediaKeys::getKeyCode(const char* keyName) {
    if (!m_display) return 0;
    
    KeySym keysym = XStringToKeysym(keyName);
    if (keysym == NoSymbol) {
        qCWarning(linuxMediaKeys) << "Не удалось найти клавишу" << keyName;
        return 0;
    }
    
    return XKeysymToKeycode(m_display, keysym);
}

bool LinuxMediaKeys::setupX11() {
    // Получаем X11 display
    m_display = QX11Info::display();
    if (!m_display) {
        qCWarning(linuxMediaKeys) << "Не удалось получить X11 display";
        return false;
    }
    
    m_rootWindow = DefaultRootWindow(m_display);
    if (!m_rootWindow) {
        qCWarning(linuxMediaKeys) << "Не удалось получить root window";
        return false;
    }
    
    // Получаем файловый дескриптор для событий
    m_x11Fd = ConnectionNumber(m_display);
    if (m_x11Fd < 0) {
        qCWarning(linuxMediaKeys) << "Не удалось получить X11 файловый дескриптор";
        return false;
    }
    
    // Создаем QSocketNotifier для асинхронной обработки событий
    m_notifier = new QSocketNotifier(m_x11Fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &LinuxMediaKeys::checkForEvents);
    
    // Захватываем медиа-клавиши
    grabMediaKeys();
    
    qCDebug(linuxMediaKeys) << "X11 настройка завершена, fd:" << m_x11Fd;
    return true;
}

void LinuxMediaKeys::cleanupX11() {
    if (m_display) {
        ungrabMediaKeys();
        m_display = nullptr;
    }
    m_rootWindow = 0;
    m_x11Fd = -1;
}

bool LinuxMediaKeys::setupWayland() {
    // TODO: Реализация Wayland через D-Bus
    qCDebug(linuxMediaKeys) << "Wayland поддержка пока не реализована";
    return false;
}

void LinuxMediaKeys::cleanupWayland() {
    // TODO: Очистка Wayland ресурсов
}

bool LinuxMediaKeys::detectDisplayServer() {
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    const char* x11Display = getenv("DISPLAY");
    
    if (waylandDisplay) {
        qCDebug(linuxMediaKeys) << "Обнаружен Wayland дисплей:" << waylandDisplay;
        return true;
    } else if (x11Display) {
        qCDebug(linuxMediaKeys) << "Обнаружен X11 дисплей:" << x11Display;
        return true;
    }
    
    qCWarning(linuxMediaKeys) << "Не обнаружен ни Wayland, ни X11 дисплей";
    return false;
}

void LinuxMediaKeys::handleKeyPress(KeyCode keycode) {
    // Получаем коды медиа-клавиш
    KeyCode playPauseKey = getKeyCode("XF86AudioPlay");
    KeyCode nextKey = getKeyCode("XF86AudioNext");
    KeyCode prevKey = getKeyCode("XF86AudioPrev");
    KeyCode stopKey = getKeyCode("XF86AudioStop");
    
    if (keycode == playPauseKey) {
        qCDebug(linuxMediaKeys) << "Play/Pause нажата";
        emit mediaKeyPressed(PlayPause);
        emit playPausePressed();
    } else if (keycode == nextKey) {
        qCDebug(linuxMediaKeys) << "Next нажата";
        emit mediaKeyPressed(Next);
        emit nextPressed();
    } else if (keycode == prevKey) {
        qCDebug(linuxMediaKeys) << "Previous нажата";
        emit mediaKeyPressed(Previous);
        emit prevPressed();
    } else if (keycode == stopKey) {
        qCDebug(linuxMediaKeys) << "Stop нажата";
        emit mediaKeyPressed(Stop);
        emit stopPressed();
    }
}

bool LinuxMediaKeys::isKeyGrabbed(KeyCode keycode) {
    if (!m_display || !m_rootWindow) return false;
    
    XKeymapEvent *keymapEvent = XGetKeyboardMapping(m_display, keycode, 1, &keycode);
    if (!keymapEvent) return false;
    
    // Проверяем, захвачена ли клавиша
    // Это упрощенная проверка, в реальности нужно более сложная логика
    XFree(keymapEvent);
    return true;
}
#endif
