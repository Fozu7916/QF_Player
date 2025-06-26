#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "../player/player.h"
#include "../model/track.h"
#include <vector>

class PlayerController
{
public:
    PlayerController();
    void saveTracks();
    void loadTracks();
    const std::vector<Track>& getTracks() const;
    void addTrack(const Track& track);
    void removeTrack(int index);
private:
    std::vector<Track> m_tracks;
};

#endif // PLAYERCONTROLLER_H