#include "playercontroller.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

PlayerController::PlayerController() : player(std::make_unique<Player>()) {}


void PlayerController::loadTracks(QString filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        tracks.clear();
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                QStringList parts = line.split(";");
                QString path = parts.value(0);
                int length = parts.value(1).toInt();
                Track track(path, length);
                tracks.push_back(track);
                emit trackLoaded(QFileInfo(path).fileName());
            }
        }
        file.close();
    }
}

void PlayerController::addTrack(const QString& filePath, int durationSec) {
    Track new_track(filePath, durationSec);
    tracks.push_back(new_track);
    emit trackLoaded(QFileInfo(filePath).fileName());
}

void PlayerController::saveTracks(QString filename) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& track : tracks) { out << track.getPath() << ";" << track.getLength() << "\n"; }
        file.close();
    }
}

void PlayerController::deleteTrack(){
    if (tracks.empty()) return;
    if ( index >= 0 and index < tracks.size()){
        player->pause();
        isPlayed = false;
        int deletedIndex = currentTrackIndex;
        tracks.erase(tracks.begin() + currentTrackIndex);
        currentTrackIndex = -1;
        emit trackDeleted(deletedIndex);
    } else {
        qWarning() << "Невозможно воспроизвести трек, проблема с индексом";
        player->setCurrentIndex(-1);
    }
}

void PlayerController::setVolume(int value) {
    if (player) player->setVolume(value);
}

void PlayerController::playNext(){
    if (tracks.empty()) return;
    if (currentTrackIndex + 1 <= tracks.size() - 1){
        player->pause();
        currentTrackIndex++;
    } else {
        currentTrackIndex = 0;
    }
    playTrackAtIndex(currentTrackIndex);
}

void PlayerController::playPrev(){
    if (tracks.empty()) return;
    if (currentTrackIndex - 1 >= 0){
        player->pause();
        currentTrackIndex--;
    } else {
        currentTrackIndex = tracks.size() - 1;
    }
    playTrackAtIndex(currentTrackIndex);
}

void PlayerController::playTrackAtIndex(int index) {
    if (tracks.empty()) return;
    if ( index >= 0 and index < tracks.size()){
        emit setCurrentRow(index);
        player->loadFile(tracks[index].getPath());
        player->play();
        isPlayed = true;
    } else {
        qWarning() << "Невозможно воспроизвести трек, проблема с индексом";
        player->setCurrentIndex(-1);
    }
}

void PlayerController::playOrStop(){
    if (currentTrackIndex >= 0 && currentTrackIndex < tracks.size()) {
        if (!isPlayed) {
            player->play();
            isPlayed = true;
            emit playOrStopUI(true);
        } else {
            player->pause();
            isPlayed = false;
            emit playOrStopUI(false);
        }
    } else {
        qWarning() << "Невозможно воспроизвести трек, проблема с индексом";
        player->setCurrentIndex(-1);
        return;
    }
}

void PlayerController::onItemClicked(int index){
        if (index >= 0 && index < tracks.size()) {
            currentTrackIndex = index;
            player->loadFile(tracks[index].getPath());
            player->play();
            isPlayed = true;
            emit setCurrentRow(currentTrackIndex);
        } else {
            qWarning() << "Невозможно выбрать трек, проблема с индексом";
            player->setCurrentIndex(-1);
        }
}

// Getters and setters
const std::vector<Track>& PlayerController::getTracks() const {
    return tracks;
}

int PlayerController::getTrackCount() const {
    return static_cast<int>(tracks.size());
}

const Track& PlayerController::getTrack(int index) const {
    return tracks.at(index);
}

void PlayerController::removeTrack(int index) {
    if (index >= 0 && index < static_cast<int>(tracks.size())) { tracks.erase(tracks.begin() + index);}
}

void PlayerController::clearTracks() {
    tracks.clear();
}

void PlayerController::setCurrentIndex(int index) {
    currentTrackIndex = index;
}

int PlayerController::getCurrentIndex() const {
    return currentTrackIndex;
}

Player* PlayerController::getPlayer() const {
    return player.get();
}


