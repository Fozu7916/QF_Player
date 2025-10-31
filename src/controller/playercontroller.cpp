#include "playercontroller.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <random>
#include <ctime>

PlayerController::PlayerController() : player(std::make_unique<Player>()) {}

static bool isNotValidTrackIndex(int index, const std::vector<Track>& tracks) {
    return index < 0 || index >= static_cast<int>(tracks.size());
}

static void playerPause(Player* player, bool& isPlayed1){
    player->pause();
    isPlayed1 = false;
}

static void playerPlay(Player* player, bool& isPlayed1){
    player->play();
    isPlayed1 = true;
}

bool PlayerController::canPlayTrack(int index) {
    if (isNotValidTrackIndex(index, tracks)) { qWarning() << "PlayerController: Invalid index:" << index; setCurrentIndex(-1); return false; }
    if (!player) { qWarning() << "PlayerController: Player не создан"; return false; }
    return true;
}


void PlayerController::loadTracks(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {qWarning()<< "PlayerController: Не удалось прочитать список треков"; return;}
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

        qInfo() << "PlayerController: Loading tracks from" << filename;
        qInfo() << "PlayerController: Tracks loaded:" << tracks.size();
}

void PlayerController::addTrack(const QString& filePath, int durationSec) {
    Track new_track(filePath, durationSec);
    tracks.push_back(new_track);
    emit trackLoaded(QFileInfo(filePath).fileName());

    qInfo() << "PlayerController: Add track" << filePath << "len" << durationSec;
}

void PlayerController::saveTracks(QString filename) {
    qInfo() << "PlayerController: Saving tracks to" << filename;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {qWarning()<< "PlayerController: Не удалось сохранить список треков"; return;}
    QTextStream out(&file);
    for (const auto& track : tracks) { out << track.getPath() << ";" << track.getLength() << "\n"; }
    file.close();
}

void PlayerController::deleteTrack(){
    if (!canPlayTrack(currentTrackIndex)) return;
    tracks.erase(tracks.begin() + currentTrackIndex);
    emit trackDeleted(currentTrackIndex);
    playTrackAtIndex(currentTrackIndex);
    playerPause(player.get(), isPlayed);
    
    qInfo() << "PlayerController: Delete track at index" << currentTrackIndex;
}

void PlayerController::playNext(){
    if (!canPlayTrack(currentTrackIndex)) return;
    playerPause(player.get(), isPlayed);
    queue.push(currentTrackIndex);
    if(isRandom){
        std::uniform_int_distribution<size_t> dist(0, tracks.size() - 1);
        size_t index = dist(engine);
        currentTrackIndex = static_cast<int>(index);
    } else{
        if (currentTrackIndex < tracks.size() - 1){
            currentTrackIndex++;
        } else {
            currentTrackIndex = 0;
            qInfo() << "PlayerController: Loop to first track";
        }
    }
    playTrackAtIndex(currentTrackIndex);
    
    qInfo() << "PlayerController: Play next from" << queue.top();
    qInfo() << "PlayerController: Play next to" << currentTrackIndex;
}

void PlayerController::playPrev(){
    if (!canPlayTrack(currentTrackIndex)) return;
    auto timePtr = std::make_unique<int>(0);
    emit KnowTime(timePtr.get());
    if(*timePtr > 1){ playTrackAtIndex(currentTrackIndex); return;}
    if(queue.size() == 0){
        if(currentTrackIndex > 0){
            currentTrackIndex--;
        }
        else {
            currentTrackIndex = tracks.size() - 1;
        }
    } else {
        qInfo() << "PlayerController: current stack index = " << queue.top();
        currentTrackIndex = queue.top();
        queue.pop();
    }
    playTrackAtIndex(currentTrackIndex);

    qInfo() << "PlayerController: Play next from" << queue.top();
    qInfo() << "PlayerController: Play next to" << currentTrackIndex;
}

void PlayerController::playTrackAtIndex(int index) {
    if (!canPlayTrack(index)) return;
    currentTrackIndex = index;
    emit setCurrentRow(index);
    player->loadFile(tracks[index].getPath());
    playerPlay(player.get(), isPlayed);
    
    qInfo() << "PlayerController: Play index" << index << QFileInfo(tracks[index].getPath()).fileName();
}

void PlayerController::playOrStop(){
    if (!canPlayTrack(currentTrackIndex)) return;
    if (isPlayed) {
        playerPause(player.get(), isPlayed);
        emit playOrStopUI(false);
        qInfo() << "PlayerController: Pause" << currentTrackIndex;
    } else {
        playerPlay(player.get(), isPlayed);
        emit playOrStopUI(true);
        qInfo() << "PlayerController: Play" << currentTrackIndex;
    }
}

void PlayerController::onItemClicked(int index){
        if (!canPlayTrack(index)) return;
        playTrackAtIndex(index);
        qInfo() << "PlayerController: Click index" << index;
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
    if (isNotValidTrackIndex(index, tracks)) return;
    tracks.erase(tracks.begin() + index);
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

void PlayerController::setPosition(int seconds) {
    if (!player) return;
    player->setPosition(seconds);
}

int PlayerController::getPosition() const {
    if (!player) return 0; 
    return player->getPosition();
}

void PlayerController::setVolume(int value) {
    if (!player) return;
    player->setVolume(value);
}