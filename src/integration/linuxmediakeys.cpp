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
    
    qDebug() << "LinuxMediaKeys: Инициализация медиа-клавиш...";
    
    // Определяем тип дисплей-сервера
    if (!detectDisplayServer()) {
        qWarning() << "LinuxMediaKeys: Не удалось определить дисплей-сервер";
        return false;
    }
    
    // Определяем приоритет инициализации
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    const char* xdgSessionType = getenv("XDG_SESSION_TYPE");
    bool preferWayland = waylandDisplay || (xdgSessionType && strcmp(xdgSessionType, "wayland") == 0);
    
    if (preferWayland) {
        // Пробуем Wayland сначала
        if (setupWayland()) {
            qDebug() << "LinuxMediaKeys: Wayland инициализация успешна";
            m_initialized = true;
            return true;
        }
        // Fallback на X11
        if (setupX11()) {
            qDebug() << "LinuxMediaKeys: X11 fallback инициализация успешна";
            m_initialized = true;
            return true;
        }
    } else {
        // Пробуем X11 сначала
        if (setupX11()) {
            qDebug() << "LinuxMediaKeys: X11 инициализация успешна";
            m_initialized = true;
            return true;
        }
        // Fallback на Wayland
        if (setupWayland()) {
            qDebug() << "LinuxMediaKeys: Wayland fallback инициализация успешна";
            m_initialized = true;
            return true;
        }
    }
    
    qWarning() << "LinuxMediaKeys: Не удалось инициализировать ни X11, ни Wayland";
    return false;
#else
    qDebug() << "LinuxMediaKeys: Медиа-клавиши не поддерживаются на этой платформе";
    return false;
#endif
}

void LinuxMediaKeys::cleanup() {
#ifdef __linux__
    if (!m_initialized) return;
    
    qDebug() << "LinuxMediaKeys: Очистка медиа-клавиш...";
    
    cleanupX11();
    cleanupWayland();
    
    if (m_notifier) {
        m_notifier->deleteLater();
        m_notifier = nullptr;
    }
    
    m_initialized = false;
    qDebug() << "LinuxMediaKeys: Очистка завершена";
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
    
    qDebug() << "LinuxMediaKeys: Захват медиа-клавиш...";
    
    // Получаем коды медиа-клавиш
    KeyCode playPauseKey = getKeyCode("XF86AudioPlay");
    KeyCode nextKey = getKeyCode("XF86AudioNext");
    KeyCode prevKey = getKeyCode("XF86AudioPrev");
    KeyCode stopKey = getKeyCode("XF86AudioStop");
    
    int grabbedCount = 0;
    
    // Захватываем клавиши с проверкой ошибок
    if (playPauseKey) {
        int result = XGrabKey(m_display, playPauseKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) { grabbedCount++; qDebug() << "LinuxMediaKeys: Play/Pause клавиша захвачена"; } else { qWarning() << "LinuxMediaKeys: Не удалось захватить Play/Pause клавишу"; }
    }
    
    if (nextKey) {
        int result = XGrabKey(m_display, nextKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) { grabbedCount++; qDebug() << "LinuxMediaKeys: Next клавиша захвачена"; } else { qWarning() << "LinuxMediaKeys: Не удалось захватить Next клавишу"; }
    }
    
    if (prevKey) {
        int result = XGrabKey(m_display, prevKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) { grabbedCount++; qDebug() << "LinuxMediaKeys: Previous клавиша захвачена"; } else { qWarning() << "LinuxMediaKeys: Не удалось захватить Previous клавишу"; }
    }
    
    if (stopKey) {
        int result = XGrabKey(m_display, stopKey, AnyModifier, m_rootWindow, False, GrabModeAsync, GrabModeAsync);
        if (result == Success) { grabbedCount++; qDebug() << "LinuxMediaKeys: Stop клавиша захвачена"; } else { qWarning() << "LinuxMediaKeys: Не удалось захватить Stop клавишу"; }
    }
    
    XFlush(m_display);
    qDebug() << "LinuxMediaKeys: Захвачено клавиш:" << grabbedCount << "из 4";
}

void LinuxMediaKeys::ungrabMediaKeys() {
    if (!m_display || !m_rootWindow) return;
    
    qDebug() << "LinuxMediaKeys: Освобождение медиа-клавиш...";
    
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
    qDebug() << "LinuxMediaKeys: Медиа-клавиши освобождены";
}

KeyCode LinuxMediaKeys::getKeyCode(const char* keyName) {
    if (!m_display) return 0;
    
    KeySym keysym = XStringToKeysym(keyName);
    if (keysym == NoSymbol) {
        qWarning() << "LinuxMediaKeys: Не удалось найти клавишу" << keyName;
        return 0;
    }
    
    return XKeysymToKeycode(m_display, keysym);
}

bool LinuxMediaKeys::setupX11() {
    // Получаем X11 display
    m_display = QX11Info::display();
    if (!m_display) {
        qWarning() << "LinuxMediaKeys: Не удалось получить X11 display";
        return false;
    }
    
    m_rootWindow = DefaultRootWindow(m_display);
    if (!m_rootWindow) {
        qWarning() << "LinuxMediaKeys: Не удалось получить root window";
        return false;
    }
    
    // Получаем файловый дескриптор для событий
    m_x11Fd = ConnectionNumber(m_display);
    if (m_x11Fd < 0) {
        qWarning() << "LinuxMediaKeys: Не удалось получить X11 файловый дескриптор";
        return false;
    }
    
    // Создаем QSocketNotifier для асинхронной обработки событий
    m_notifier = new QSocketNotifier(m_x11Fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &LinuxMediaKeys::checkForEvents);
    
    // Захватываем медиа-клавиши
    grabMediaKeys();
    
    qDebug() << "LinuxMediaKeys: X11 настройка завершена, fd:" << m_x11Fd;
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
    qDebug() << "LinuxMediaKeys: Инициализация Wayland через D-Bus...";
    
    // Проверяем доступность D-Bus
    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning() << "LinuxMediaKeys: D-Bus сессия недоступна";
        return false;
    }
    
    // Подключаемся к org.freedesktop.portal.MediaKeys
    QString service = "org.freedesktop.portal.Desktop";
    QString path = "/org/freedesktop/portal/desktop";
    QString interface = "org.freedesktop.portal.MediaKeys";
    
    QDBusInterface dbusInterface(service, path, interface, QDBusConnection::sessionBus(), this);
    
    if (!dbusInterface.isValid()) {
        qWarning() << "LinuxMediaKeys: Не удалось подключиться к D-Bus интерфейсу MediaKeys";
        return false;
    }
    
    // Регистрируем приложение для получения медиа-событий
    QDBusMessage registerCall = dbusInterface.call("RegisterMediaKeys", QApplication::applicationName());
    
    if (registerCall.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "LinuxMediaKeys: Не удалось зарегистрировать медиа-клавиши:" << registerCall.errorMessage();
        return false;
    }
    
    // Подключаемся к сигналам медиа-клавиш
    QDBusConnection::sessionBus().connect(
        service, path, interface, "MediaKeyPressed",
        this, SLOT(onMediaKeyPressed(QString, QVariantMap))
    );
    
    qDebug() << "LinuxMediaKeys: Wayland медиа-клавиши успешно инициализированы";
    return true;
}

void LinuxMediaKeys::cleanupWayland() {
    qDebug() << "LinuxMediaKeys: Очистка Wayland ресурсов...";
    
    if (QDBusConnection::sessionBus().isConnected()) {
        QString service = "org.freedesktop.portal.Desktop";
        QString path = "/org/freedesktop/portal/desktop";
        QString interface = "org.freedesktop.portal.MediaKeys";
        
        QDBusInterface dbusInterface(service, path, interface, QDBusConnection::sessionBus(), this);
        
        if (dbusInterface.isValid()) {
            // Отменяем регистрацию медиа-клавиш
            QDBusMessage unregisterCall = dbusInterface.call("UnregisterMediaKeys", QApplication::applicationName());
            
            if (unregisterCall.type() == QDBusMessage::ErrorMessage) {
                qWarning() << "LinuxMediaKeys: Ошибка при отмене регистрации медиа-клавиш:" << unregisterCall.errorMessage();
            }
        }
        
        // Отключаемся от сигналов
        QDBusConnection::sessionBus().disconnect(
            service, path, interface, "MediaKeyPressed",
            this, SLOT(onMediaKeyPressed(QString, QVariantMap))
        );
    }
    
    qDebug() << "LinuxMediaKeys: Wayland ресурсы очищены";
}

bool LinuxMediaKeys::detectDisplayServer() {
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    const char* x11Display = getenv("DISPLAY");
    const char* xdgSessionType = getenv("XDG_SESSION_TYPE");
    
    // Приоритет: Wayland > X11
    if (waylandDisplay || (xdgSessionType && strcmp(xdgSessionType, "wayland") == 0)) {
        qDebug() << "LinuxMediaKeys: Обнаружен Wayland дисплей:" << (waylandDisplay ? waylandDisplay : "через XDG_SESSION_TYPE");
        return true;
    } else if (x11Display) {
        qDebug() << "LinuxMediaKeys: Обнаружен X11 дисплей:" << x11Display;
        return true;
    }
    
    qWarning() << "LinuxMediaKeys: Не обнаружен ни Wayland, ни X11 дисплей";
    return false;
}

void LinuxMediaKeys::handleKeyPress(KeyCode keycode) {
    // Получаем коды медиа-клавиш
    KeyCode playPauseKey = getKeyCode("XF86AudioPlay");
    KeyCode nextKey = getKeyCode("XF86AudioNext");
    KeyCode prevKey = getKeyCode("XF86AudioPrev");
    KeyCode stopKey = getKeyCode("XF86AudioStop");
    
    if (keycode == playPauseKey) {
        qDebug() << "LinuxMediaKeys: Play/Pause нажата";
        emit mediaKeyPressed(PlayPause);
        emit playPausePressed();
    } else if (keycode == nextKey) {
        qDebug() << "LinuxMediaKeys: Next нажата";
        emit mediaKeyPressed(Next);
        emit nextPressed();
    } else if (keycode == prevKey) {
        qDebug() << "LinuxMediaKeys: Previous нажата";
        emit mediaKeyPressed(Previous);
        emit prevPressed();
    } else if (keycode == stopKey) {
        qDebug() << "LinuxMediaKeys: Stop нажата";
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

void LinuxMediaKeys::onMediaKeyPressed(const QString& key, const QVariantMap& metadata) {
    qDebug() << "LinuxMediaKeys: Wayland медиа-клавиша нажата:" << key;
    
    // Обрабатываем различные типы медиа-клавиш
    if (key == "Play" || key == "Pause" || key == "PlayPause") {
        qDebug() << "LinuxMediaKeys: Play/Pause через Wayland";
        emit mediaKeyPressed(PlayPause);
        emit playPausePressed();
    } else if (key == "Next") {
        qDebug() << "LinuxMediaKeys: Next через Wayland";
        emit mediaKeyPressed(Next);
        emit nextPressed();
    } else if (key == "Previous") {
        qDebug() << "LinuxMediaKeys: Previous через Wayland";
        emit mediaKeyPressed(Previous);
        emit prevPressed();
    } else if (key == "Stop") {
        qDebug() << "LinuxMediaKeys: Stop через Wayland";
        emit mediaKeyPressed(Stop);
        emit stopPressed();
    } else {
        qDebug() << "LinuxMediaKeys: Неизвестная медиа-клавиша:" << key;
    }
}

QString LinuxMediaKeys::getStatus() const {
    if (!m_initialized) {
        return "Не инициализирован";
    }
    
    if (m_display) {
        return "X11 активен";
    } else if (QDBusConnection::sessionBus().isConnected()) {
        return "Wayland активен";
    }
    
    return "Неизвестное состояние";
}
#endif
