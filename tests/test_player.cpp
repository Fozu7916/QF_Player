#include <QtTest/QtTest>
#include "../src/player/player.h"

class TestPlayer : public QObject {
    Q_OBJECT
private slots:
    void createAndCallMethods();
    void eofWithoutFile_isFalse();
};

void TestPlayer::createAndCallMethods() {
    Player p;
    p.setVolume(50);
    p.setPosition(10);
    // Проверяем, что методы не падают
    QVERIFY(true);
}

void TestPlayer::eofWithoutFile_isFalse() {
    Player p;
    QVERIFY(p.isEof() == false);
    QVERIFY(p.getPosition() == 0);
}

QTEST_MAIN(TestPlayer)
#include "test_player.moc" 