#include "playercontroller.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <random>
#include <ctime>

PlayerController::PlayerController() : player(std::make_unique<Player>()) {}


void PlayerController::loadTracks(QString filename) {
    qInfo() << "Loading tracks from" << filename;
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
        qInfo() << "Tracks loaded:" << tracks.size();
    }
}

void PlayerController::addTrack(const QString& filePath, int durationSec) {
    qInfo() << "Add track" << filePath << "len" << durationSec;
    Track new_track(filePath, durationSec);
    tracks.push_back(new_track);
    emit trackLoaded(QFileInfo(filePath).fileName());
}

void PlayerController::saveTracks(QString filename) {
    qInfo() << "Saving tracks to" << filename;
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& track : tracks) { out << track.getPath() << ";" << track.getLength() << "\n"; }
        file.close();
    }
}

void PlayerController::deleteTrack(){
    if (tracks.empty()) return;
    if ( currentTrackIndex >= 0 and currentTrackIndex < tracks.size()){
        qInfo() << "Delete track at index" << currentTrackIndex;
        player->pause();
        isPlayed = false;
        int deletedIndex = currentTrackIndex;
        tracks.erase(tracks.begin() + currentTrackIndex);
        currentTrackIndex = -1;
        emit trackDeleted(deletedIndex);
    } else {
        qWarning() << "Невозможно воспроизвести трек, проблема с индексом";
        setCurrentIndex(-1);
    }
}

void PlayerController::setVolume(int value) {
    if (player) player->setVolume(value);
}

void PlayerController::setPosition(int seconds) {
    if (player) player->setPosition(seconds);
}

int PlayerController::getPosition() const {
    if (player) return player->getPosition();
    return 0;
}

void PlayerController::playNext(){
    if (tracks.empty()) return;
    if(isRandom){
        static std::random_device rd;
        static std::mt19937 engine(rd());
        std::uniform_int_distribution<size_t> dist(0, tracks.size() - 1);
        size_t index = dist(engine);
        currentTrackIndex = static_cast<int>(index);
    } else{
        if (currentTrackIndex + 1 <= tracks.size() - 1){
            qInfo() << "Play next from" << currentTrackIndex;
            player->pause();
            currentTrackIndex++;
        } else {
            qInfo() << "Loop to first track";
            currentTrackIndex = 0;
        }
    }
    playTrackAtIndex(currentTrackIndex);
}

void PlayerController::playPrev(){
    if (tracks.empty()) return;
    if (currentTrackIndex - 1 >= 0){
        qInfo() << "Play prev from" << currentTrackIndex;
        player->pause();
        currentTrackIndex--;
    } else {
        qInfo() << "Loop to last track";
        currentTrackIndex = tracks.size() - 1;
    }
    playTrackAtIndex(currentTrackIndex);
}

void PlayerController::playTrackAtIndex(int index) {
    if (tracks.empty()) return;
    if ( index >= 0 and index < tracks.size()){
        qInfo() << "Play index" << index << QFileInfo(tracks[index].getPath()).fileName();
        emit setCurrentRow(index);
        player->loadFile(tracks[index].getPath());
        player->play();
        isPlayed = true;
    } else {
        qWarning() << "Невозможно воспроизвести трек, проблема с индексом";
        setCurrentIndex(-1);
    }
}

void PlayerController::playOrStop(){
    if (currentTrackIndex >= 0 && currentTrackIndex < tracks.size()) {
        if (!isPlayed) {
            qInfo() << "Play" << currentTrackIndex;
            player->play();
            isPlayed = true;
            emit playOrStopUI(true);
        } else {
            qInfo() << "Pause" << currentTrackIndex;
            player->pause();
            isPlayed = false;
            emit playOrStopUI(false);
        }
    } else {
        qWarning() << "Невозможно воспроизвести трек, проблема с индексом";
        setCurrentIndex(-1);
        return;
    }
}

void PlayerController::onItemClicked(int index){
        if (index >= 0 && index < tracks.size()) {
            qInfo() << "Click index" << index;
            currentTrackIndex = index;
            player->loadFile(tracks[index].getPath());
            player->play();
            isPlayed = true;
            emit setCurrentRow(currentTrackIndex);
        } else {
            qWarning() << "Невозможно выбрать трек, проблема с индексом";
            setCurrentIndex(-1);
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

void PlayerController::setRandom(bool now) {
    isRandom = now;
}
bool PlayerController::getRandom() const{
    return isRandom;
}

void PlayerController::setPlayed(bool now){
    isPlayed = now;
}
bool PlayerController::getPlayed() const{
    return isPlayed;
}