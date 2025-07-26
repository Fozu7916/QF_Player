#include <QtTest/QtTest>
#include "../src/controller/playercontroller.h"
#include <QObject>
#include <QSignalSpy>
#include <QTemporaryFile>

// Мок-версия Player для тестов (без mpv)
class MockPlayer : public Player {
public:
    MockPlayer() : Player(nullptr) {}
    void loadFile(const QString&) {}
    void play() { played = true; }
    void pause() { played = false; }
    void setVolume(int) {}
    int getPosition() { return 0; }
    void setPosition(int) {}
    bool played = false;
};

class TestPlayerController : public QObject {
    Q_OBJECT
private slots:
    void addTrack_increasesCount();
    void addTrack_correctPath();
    void addTrack_correctLength();
    void removeTrack_decreasesCount();
    void removeTrack_correctTrackRemains();
    void clearTracks_emptiesList();
    void setCurrentIndex_setsValue();
    void getCurrentIndex_returnsValue();
    void signal_trackLoaded_emitted();
    void signal_trackLoaded_correctName();
    void signal_trackDeleted_emitted();
    void signal_trackDeleted_correctIndex();
    void playNext_emptyList_noCrash();
    void playPrev_emptyList_noCrash();
    void playOrStop_emptyList_noCrash();
    void saveTracks_createsFile();
    void loadTracks_loadsCorrectly();
};

void TestPlayerController::addTrack_increasesCount() {
    PlayerController pc;
    int before = pc.getTrackCount();
    pc.addTrack("/music/track1.mp3", 100);
    QCOMPARE(pc.getTrackCount(), before + 1);
}
void TestPlayerController::addTrack_correctPath() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    QCOMPARE(pc.getTrack(0).getPath(), QString("/music/track1.mp3"));
}
void TestPlayerController::addTrack_correctLength() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 123);
    QCOMPARE(pc.getTrack(0).getLength(), 123);
}
void TestPlayerController::removeTrack_decreasesCount() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    pc.addTrack("/music/track2.mp3", 200);
    int before = pc.getTrackCount();
    pc.removeTrack(0);
    QCOMPARE(pc.getTrackCount(), before - 1);
}
void TestPlayerController::removeTrack_correctTrackRemains() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    pc.addTrack("/music/track2.mp3", 200);
    pc.removeTrack(0);
    QCOMPARE(pc.getTrack(0).getPath(), QString("/music/track2.mp3"));
}
void TestPlayerController::clearTracks_emptiesList() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    pc.clearTracks();
    QCOMPARE(pc.getTrackCount(), 0);
}
void TestPlayerController::setCurrentIndex_setsValue() {
    PlayerController pc;
    pc.setCurrentIndex(2);
    QCOMPARE(pc.getCurrentIndex(), 2);
}
void TestPlayerController::getCurrentIndex_returnsValue() {
    PlayerController pc;
    pc.setCurrentIndex(5);
    QCOMPARE(pc.getCurrentIndex(), 5);
}
void TestPlayerController::signal_trackLoaded_emitted() {
    PlayerController pc;
    QSignalSpy spy(&pc, SIGNAL(trackLoaded(QString)));
    pc.addTrack("/music/track1.mp3", 100);
    QCOMPARE(spy.count(), 1);
}
void TestPlayerController::signal_trackLoaded_correctName() {
    PlayerController pc;
    QSignalSpy spy(&pc, SIGNAL(trackLoaded(QString)));
    pc.addTrack("/music/track1.mp3", 100);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("track1.mp3"));
}
void TestPlayerController::signal_trackDeleted_emitted() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    pc.setCurrentIndex(0);
    QSignalSpy spy(&pc, SIGNAL(trackDeleted(int)));
    pc.deleteTrack();
    QCOMPARE(spy.count(), 1);
}
void TestPlayerController::signal_trackDeleted_correctIndex() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    pc.setCurrentIndex(0);
    QSignalSpy spy(&pc, SIGNAL(trackDeleted(int)));
    pc.deleteTrack();
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toInt(), 0);
}
void TestPlayerController::playNext_emptyList_noCrash() {
    PlayerController pc;
    pc.playNext();
    QVERIFY(true);
}
void TestPlayerController::playPrev_emptyList_noCrash() {
    PlayerController pc;
    pc.playPrev();
    QVERIFY(true);
}
void TestPlayerController::playOrStop_emptyList_noCrash() {
    PlayerController pc;
    pc.playOrStop();
    QVERIFY(true);
}
void TestPlayerController::saveTracks_createsFile() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    QTemporaryFile file;
    QVERIFY(file.open());
    QString fname = file.fileName();
    pc.saveTracks(fname);
    QFile checkFile(fname);
    QVERIFY(checkFile.exists());
}
void TestPlayerController::loadTracks_loadsCorrectly() {
    PlayerController pc;
    pc.addTrack("/music/track1.mp3", 100);
    pc.addTrack("/music/track2.mp3", 200);
    QTemporaryFile file;
    QVERIFY(file.open());
    QString fname = file.fileName();
    pc.saveTracks(fname);
    PlayerController pc2;
    pc2.loadTracks(fname);
    QCOMPARE(pc2.getTrackCount(), 2);
    QCOMPARE(pc2.getTrack(0).getPath(), QString("/music/track1.mp3"));
    QCOMPARE(pc2.getTrack(1).getLength(), 200);
}

QTEST_MAIN(TestPlayerController)
#include "test_playercontroller.moc" 