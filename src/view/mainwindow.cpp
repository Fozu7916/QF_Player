#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QtMultimedia/QMediaPlayer>
#include <QEventLoop>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "../controller/playercontroller.h"

const int DEFAULT_VOLUME = 50;
const QString PLAYLIST_FILENAME = "tracks.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sliderTimer(nullptr)
    , playerController(std::make_unique<PlayerController>())
{
    ui->setupUi(this);
    if (statusBar()) statusBar()->hide();

    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(DEFAULT_VOLUME);
    ui->volume->setText(QString::number(DEFAULT_VOLUME));

    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value){
        ui->volume->setText(QString::number(value));
        playerController->setVolume(value);
    });
    connect(playerController.get(), &PlayerController::trackDeleted, this, &MainWindow::deleteTrackFromList);
    connect(playerController.get(), &PlayerController::trackLoaded, this, &MainWindow::addTrackToList);
    connect(playerController.get(), &PlayerController::setCurrentRow, this, &MainWindow::setCurrentRow);
    connect(playerController.get(), &PlayerController::playOrStopUI, this, &MainWindow::onPlayOrStopUI);

    playerController->loadTracks(PLAYLIST_FILENAME);
}

MainWindow::~MainWindow(){
    playerController->saveTracks(PLAYLIST_FILENAME);
    delete ui;
}

void MainWindow::on_playOrStopButton_clicked(){
    playerController->playOrStop();
}

void MainWindow::on_addButton_clicked(){
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите трек", "", "Аудио файлы (*.mp3 *.wav *.flac)");
    if (!filePath.isEmpty()) playerController->addTrack(filePath,getDuration(filePath));
}

int MainWindow::getDuration(QString filePath){
        QMediaPlayer mediaPlayer;
        mediaPlayer.setSource(QUrl::fromLocalFile(filePath));
        QEventLoop loop;
        QObject::connect(&mediaPlayer, &QMediaPlayer::durationChanged, &loop, &QEventLoop::quit);
        mediaPlayer.play();
        loop.exec();
        qint64 durationMs = mediaPlayer.duration();
        mediaPlayer.stop();
        int durationSec = static_cast<int>(durationMs / 1000);
        return durationSec;
}

void MainWindow::on_deleteButton_clicked(){
    playerController->deleteTrack();
}

void MainWindow::on_nextButton_clicked(){
    playerController->playNext();
}

void MainWindow::on_prevButton_clicked(){
    playerController->playPrev();
}

void MainWindow::on_horizontalSlider_sliderMoved(int position){
    playerController->getPlayer()->setPosition(position);
    int min = position / 60;
    int sec = position % 60;
    ui->time->setText(QString::asprintf("%d:%02d", min, sec));
}

void MainWindow::on_TrackLists_itemClicked(QListWidgetItem *item){
        int index = ui->trackList->row(item);
        playerController->onItemClicked(index);

        ui->horizontalSlider->setMaximum(playerController->getTracks()[index].getLength());
        if (!sliderTimer) {
            sliderTimer = new QTimer(this);
            connect(sliderTimer, &QTimer::timeout, this, [this]() {
                if (playerController->getCurrentIndex() >= 0 && playerController->getCurrentIndex() < playerController->getTracks().size()) {
                    int pos = playerController->getPlayer()->getPosition();
                    ui->horizontalSlider->setValue(pos);
                    int min = pos / 60;
                    int sec = pos % 60;
                    ui->time->setText(QString::asprintf("%d:%02d", min, sec));
                }
            });
        }

        sliderTimer->start(500);
}

void MainWindow::addTrackToList(const QString& name) {
    ui->trackList->addItem(name);
}


void MainWindow::deleteTrackFromList(int index) {
    if (index >= 0 && index < ui->trackList->count())
        ui->trackList->takeItem(index);
}
    
void MainWindow::setCurrentRow(int index) {
    if (index >= 0 && index < ui->trackList->count()) {
        ui->trackList->setCurrentRow(index);
    }
}

void MainWindow::onPlayOrStopUI(bool isPlaying) {
    if (isPlaying) {
        ui->playOrStopButton->setText("⏸");
    } else {
        ui->playOrStopButton->setText("▶");
    }
}
