#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "../../mpv/include/mpv/client.h"

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    void loadFile(const QString &filePath);
    void play();
    void pause();
    void setVolume(int volume);
    int getPosition();
    void setPosition(int seconds);

private:
    mpv_handle *mpv;
};

#endif // PLAYER_H
