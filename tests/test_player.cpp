#include <QtTest/QtTest>
#include "../src/player/player.h"

class TestPlayer : public QObject {
    Q_OBJECT
private slots:
    void createAndCallMethods();
};

void TestPlayer::createAndCallMethods() {
    Player p;
    p.setVolume(50);
    p.setPosition(10);
    // Проверяем, что методы не падают
    QVERIFY(true);
}

QTEST_MAIN(TestPlayer)
#include "test_player.moc" 