#include <QtTest/QtTest>
#include "../src/model/track.h"

class TestTrack : public QObject {
    Q_OBJECT
private slots:
    void parameterizedConstructor();
};

void TestTrack::parameterizedConstructor() {
    Track t("/music/song.mp3", 123);
    QCOMPARE(t.getPath(), QString("/music/song.mp3"));
    QCOMPARE(t.getLength(), 123);
}

QTEST_MAIN(TestTrack)
#include "test_track.moc" 
