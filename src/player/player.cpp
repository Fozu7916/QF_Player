#include "player.h"
#include <QDebug>

Player::Player(QObject *parent) : QObject(parent)
{
    mpv = mpv_create();
    if (!mpv) {
        qFatal("Could not create mpv context");
        return;
    }

    mpv_set_option_string(mpv, "input-default-bindings", "yes");
    mpv_set_option_string(mpv, "vo", "null"); // No video output

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
    const char *cmd[] = {"loadfile", filePath.toUtf8().constData(), nullptr};
    mpv_command(mpv, cmd);
}

void Player::play()
{
    mpv_set_property_string(mpv, "pause", "no");
}

void Player::pause()
{
    mpv_set_property_string(mpv, "pause", "yes");
}

void Player::setVolume(int volume)
{
    mpv_set_property_string(mpv, "volume", QString::number(volume).toUtf8().constData());
}

int Player::getPosition() {
    if (!mpv) return 0;
    double pos = 0;
    mpv_get_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos);
    return static_cast<int>(pos);
}

void Player::setPosition(int seconds) {
    if (!mpv) return;
    double pos = static_cast<double>(seconds);
    mpv_set_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos);
}
