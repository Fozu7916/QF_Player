#pragma once

#include <QObject>

class PlayerController;

class MprisService : public QObject {
    Q_OBJECT
public:
    explicit MprisService(PlayerController* controller, QObject* parent = nullptr);
    ~MprisService() override = default;

private:
    PlayerController* m_controller;
    void registerOnSessionBus();
};


