#include "player.h"
#include <QDebug>

Player::Player(QObject *parent) : QObject(parent)
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    std::setlocale(LC_NUMERIC, "C");
#endif
    mpv = mpv_create();
    if (!mpv) {
        qFatal("Could not create mpv context");
        return;
    }

    mpv_set_option_string(mpv, "input-default-bindings", "yes");
    mpv_set_option_string(mpv, "vo", "null"); // Без видеовыхода

#if defined(__linux__) || defined(__unix__)
    mpv_set_option_string(mpv, "ao", "pulse"); // Linux: PulseAudio
#endif
    if (mpv_initialize(mpv) < 0) {
        qFatal("Could not initialize mpv");
    }
}

Player::~Player()
{
    if (mpv) {
        mpv_terminate_destroy(mpv);
    }
}

void Player::loadFile(const QString &filePath)
{
    const QByteArray pathUtf8 = filePath.toUtf8();
    const char *cmd[] = {"loadfile", pathUtf8.constData(), nullptr};
    int status = mpv_command(mpv, cmd);
    if (status < 0) {
        qWarning() << "Player: Ошибка загрузки файла, статус" << status << "для" << filePath;
    }
}

void Player::play()
{
    int status = mpv_set_property_string(mpv, "pause", "no");
    if (status < 0) {
        qWarning() << "Player: Ошибка воспроизведения, статус" << status;
    }
}

void Player::pause()
{
    int status = mpv_set_property_string(mpv, "pause", "yes");
    if (status < 0) {
        qWarning() << "Player: Ошибка паузы, статус" << status;
    }
}

void Player::setVolume(int volume)
{
    const QByteArray vol = QString::number(volume).toUtf8();
    int status = mpv_set_property_string(mpv, "volume", vol.constData());
    if (status < 0) {
        qWarning() << "Player: Ошибка установки громкости, статус" << status << "значение" << volume;
    }
}

int Player::getPosition() {
    if (!mpv) return 0;
    double pos = 0;
    int status = mpv_get_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos);
    if (status < 0) return 0; // Возвращаем 0 при ошибке чтения позиции
    return static_cast<int>(pos);
}

void Player::setPosition(int seconds) {
    if (!mpv) return;
    double pos = static_cast<double>(seconds);
    int status = mpv_set_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos);
    if (status < 0) {
        qWarning() << "Player: Ошибка установки позиции, статус" << status << "секунды" << seconds;
    }
}

bool Player::isEof() {
    if (!mpv) return false;
    int flag = 0;
    int status = mpv_get_property(mpv, "eof-reached", MPV_FORMAT_FLAG, &flag);
    if (status < 0) return false;
    return flag != 0;
}
