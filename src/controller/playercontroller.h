#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "../player/player.h"
#include "../model/track.h"
#include <vector>
#include <memory>
#include <QObject>

class PlayerController : public QObject
{
    Q_OBJECT
public:
    PlayerController();
    void saveTracks();
    void loadTracks();
    void addTrack(const QString& filePath, int durationSec);
    void deleteTrack();
    void OnItemClicked(int index);
    void playprev();
    void playnext();
    void playOrStop();
    void setVolume(int value);
    // гетеры и сетеры для всех переменных
    const std::vector<Track>& getTracks() const;
    int getTrackCount() const;
    const Track& getTrack(int index) const;
    void removeTrack(int index);
    void clearTracks();
    void setCurrentIndex(int index);
    int getCurrentIndex() const;
    Player* getPlayer() const;
signals:
    void trackLoaded(const QString& name);
    void trackDeleted(int index);
    void setCurrentRow(int index);
    void playOrStopUI(bool isPlaying);
private:
    std::vector<Track> m_tracks;
    std::unique_ptr<Player> m_player;
    bool isPlayed;
    int currentTrackIndex = -1;
};

#endif // PLAYERCONTROLLER_H