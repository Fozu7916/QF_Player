#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "../player/player.h"
#include "../model/track.h"
#include <vector>
#include <memory>
#include <QObject>
#include <random>

class PlayerController : public QObject
{
    Q_OBJECT
public:
    PlayerController();
    void saveTracks(QString filename);
    void loadTracks(QString filename);
    void addTrack(const QString& filePath, int durationSec);
    void deleteTrack();
    void onItemClicked(int index);
    void playPrev();
    void playNext();
    void playOrStop();
    void setVolume(int value);
    bool canPlayTrack(int index);
    // Getters and setters for all variables
    void setPosition(int seconds);
    int getPosition() const;
    const std::vector<Track>& getTracks() const;
    int getTrackCount() const;
    const Track& getTrack(int index) const;
    void removeTrack(int index);
    void clearTracks();
    void setCurrentIndex(int index);
    int getCurrentIndex() const;
    Player* getPlayer() const;
    void setRandom(bool now);
    bool getRandom() const;
    void setPlayed(bool now);
    bool getPlayed() const;
signals:
    void trackLoaded(const QString& name);
    void trackDeleted(int index);
    void setCurrentRow(int index);
    void playOrStopUI(bool isPlaying);
private:
    std::vector<Track> tracks;
    std::unique_ptr<Player> player;
    bool isPlayed = false;
    bool isRandom = false;
    int currentTrackIndex = -1;
    void playTrackAtIndex(int index);
    std::mt19937 engine{std::random_device{}()};
};

#endif // PLAYERCONTROLLER_H