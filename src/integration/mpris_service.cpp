#include "mpris_service.h"
#include "../controller/playercontroller.h"

#ifdef HAVE_MPRIS
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusConnectionInterface>
#endif

MprisService::MprisService(PlayerController* controller, QObject* parent)
    : QObject(parent), m_controller(controller) {
#ifdef HAVE_MPRIS
    registerOnSessionBus();
#endif
}

void MprisService::registerOnSessionBus() {
#ifdef HAVE_MPRIS
    auto& bus = QDBusConnection::sessionBus();
    const QString service = "org.mpris.MediaPlayer2.qf_player";
    const QString path = "/org/mpris/MediaPlayer2";

    bus.unregisterService(service);
    if (!bus.registerService(service)) return;

    bus.registerObject(path, this, QDBusConnection::ExportAllSlots);

    bus.connect({}, {}, {}, {}, this, SLOT());

    bus.registerVirtualObject(path, QDBusConnection::VirtualObjectRouting::SubPath,
        [this](const QDBusMessage &message) {
            const QString iface = message.interface();
            const QString member = message.member();
            if (iface == "org.mpris.MediaPlayer2.Player") {
                if (member == "PlayPause") { m_controller->playOrStop(); }
                else if (member == "Play") { if (!m_controller) return QDBusMessage(); m_controller->playOrStop(); }
                else if (member == "Pause") { if (!m_controller) return QDBusMessage(); m_controller->playOrStop(); }
                else if (member == "Next") { m_controller->playNext(); }
                else if (member == "Previous") { m_controller->playPrev(); }
            }
            return QDBusMessage();
        }
    );
#endif
}


