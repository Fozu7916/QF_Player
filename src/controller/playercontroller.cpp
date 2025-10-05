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
    qInfo() << "Loading tracks from" << filename;
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
        qInfo() << "PlayerController: Tracks loaded:" << tracks.size();
}

void PlayerController::addTrack(const QString& filePath, int durationSec) {
    qInfo() << "PlayerController: Add track" << filePath << "len" << durationSec;
    Track new_track(filePath, durationSec);
    tracks.push_back(new_track);
    emit trackLoaded(QFileInfo(filePath).fileName());
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
    qInfo() << "PlayerController: Delete track at index" << currentTrackIndex;
    int deletedIndex = currentTrackIndex;
    tracks.erase(tracks.begin() + currentTrackIndex);
    emit trackDeleted(deletedIndex);
    playTrackAtIndex(currentTrackIndex);
    playerPause(player.get(), isPlayed);
}

void PlayerController::playNext(){
    if (!canPlayTrack(currentTrackIndex)) return;
    playerPause(player.get(), isPlayed);
    if(isRandom){
        qInfo() << "PlayerController: Play next from" << currentTrackIndex;
        std::uniform_int_distribution<size_t> dist(0, tracks.size() - 1);
        size_t index = dist(engine);
        currentTrackIndex = static_cast<int>(index);
        qInfo() << "PlayerController: Play next to" << currentTrackIndex;
    } else{
        if (currentTrackIndex < tracks.size() - 1){
            qInfo() << "PlayerController: Play next from" << currentTrackIndex;
            currentTrackIndex++;
            qInfo() << "PlayerController: Play next to" << currentTrackIndex;
        } else {
            qInfo() << "PlayerController: Loop to first track";
            currentTrackIndex = 0;
        }
    }
    playTrackAtIndex(currentTrackIndex);
}

void PlayerController::playPrev(){
    if (!canPlayTrack(currentTrackIndex)) return;
    if (currentTrackIndex < 1){
        qInfo() << "PlayerController:Loop to last track";
        currentTrackIndex = tracks.size() - 1;
    } else{
        qInfo() << "PlayerController: Play prev from" << currentTrackIndex;
        player->pause();
        currentTrackIndex--;
    }
    playTrackAtIndex(currentTrackIndex);
}

void PlayerController::playTrackAtIndex(int index) {
    if (!canPlayTrack(index)) return;
    qInfo() << "PlayerController: Play index" << index << QFileInfo(tracks[index].getPath()).fileName();
    emit setCurrentRow(index);
    player->loadFile(tracks[index].getPath());
    playerPlay(player.get(), isPlayed);
}

void PlayerController::playOrStop(){
    if (!canPlayTrack(currentTrackIndex)) return;
    if (isPlayed) {
        qInfo() << "PlayerController: Pause" << currentTrackIndex;
        playerPause(player.get(), isPlayed);
        emit playOrStopUI(false);
    } else {
        qInfo() << "PlayerController: Play" << currentTrackIndex;
        playerPlay(player.get(), isPlayed);
        emit playOrStopUI(true);
    }
}

void PlayerController::onItemClicked(int index){
        if (!canPlayTrack(index)) return;
        qInfo() << "PlayerController: Click index" << index;
        currentTrackIndex = index;
        player->loadFile(tracks[index].getPath());
        playerPlay(player.get(), isPlayed);
        emit setCurrentRow(currentTrackIndex);
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