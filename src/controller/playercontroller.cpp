#include "playercontroller.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

PlayerController::PlayerController() : m_player(std::make_unique<Player>()) {
    m_player->setVolume(50);
}

void PlayerController::loadTracks() {
    QFile file("tracks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        m_tracks.clear();
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                QStringList parts = line.split(";");
                QString path = parts.value(0);
                int length = parts.value(1).toInt();
                Track track(path, length);
                m_tracks.push_back(track);
                emit trackLoaded(QFileInfo(path).fileName());
            }
        }
        file.close();
    }
}

void PlayerController::addTrack(const QString& filePath, int durationSec) {
    Track new_track(filePath, durationSec);
    m_tracks.push_back(new_track);
    emit trackLoaded(QFileInfo(filePath).fileName());
}

void PlayerController::saveTracks() {
    QFile file("tracks.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& track : m_tracks) {
            out << track.getPath() << ";" << track.getLength() << "\n";
        }
        file.close();
    }
}


void PlayerController::deleteTrack()
{
    m_player->pause();
    isPlayed = false;
    int deletedIndex = currentTrackIndex;
    m_tracks.erase(m_tracks.begin() + currentTrackIndex);
    currentTrackIndex = -1;
    emit trackDeleted(deletedIndex);
}

const std::vector<Track>& PlayerController::getTracks() const {
    return m_tracks;
}

int PlayerController::getTrackCount() const {
    return static_cast<int>(m_tracks.size());
}

const Track& PlayerController::getTrack(int index) const {
    return m_tracks.at(index);
}

void PlayerController::removeTrack(int index) {
    if (index >= 0 && index < static_cast<int>(m_tracks.size())) {
        m_tracks.erase(m_tracks.begin() + index);
    }
    emit setCurrentRow(currentTrackIndex -1);
}

void PlayerController::clearTracks() {
    m_tracks.clear();
}

void PlayerController::setCurrentIndex(int index) {
    currentTrackIndex = index;
}

int PlayerController::getCurrentIndex() const {
    return currentTrackIndex;
}

void PlayerController::OnItemClicked(int index){
        if (index >= 0 && index < m_tracks.size()) {
        currentTrackIndex = index;
        m_player->loadFile(m_tracks[index].getPath());
        m_player->play();
        isPlayed = true;
        emit setCurrentRow(currentTrackIndex);
        }
}

Player* PlayerController::getPlayer() const {
    return m_player.get();
}

void PlayerController::playprev(){
    if(currentTrackIndex  - 1  >= 0){
        m_player->pause();
        currentTrackIndex--;
        emit setCurrentRow(currentTrackIndex);
        m_player->loadFile(m_tracks[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }else{
        currentTrackIndex = m_tracks.size() - 1;
        emit setCurrentRow(currentTrackIndex);
        m_player->loadFile(m_tracks[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }

}

void PlayerController::playnext(){
    if(currentTrackIndex + 1 <= m_tracks.size() - 1){
        m_player->pause();
        currentTrackIndex++;
        emit setCurrentRow(currentTrackIndex);
        m_player->loadFile(m_tracks[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }else{
        currentTrackIndex = 0;
        emit setCurrentRow(currentTrackIndex);
        m_player->loadFile(m_tracks[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }
}

void PlayerController::playOrStop()
{
    if (currentTrackIndex < 0 || currentTrackIndex >= m_tracks.size()) return;
    if (!isPlayed) {
        m_player->play();
        isPlayed = true;
        emit playOrStopUI(true);
    } else {
        m_player->pause();
        isPlayed = false;
        emit playOrStopUI(false);
    }
}
void PlayerController::setVolume(int value) {
    if (m_player) m_player->setVolume(value);
}
