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
#include "../changebackground.h"
#include "../controller/playercontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sliderTimer(nullptr)
    , playerController(std::make_unique<PlayerController>())
{
    ui->setupUi(this);
    changebackground(this,":/images/fone.jpg");
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50);
    ui->volume->setText(QString::number(50));
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value){
        ui->volume->setText(QString::number(value));
        playerController->setVolume(value);
    });

    connect(playerController.get(), &PlayerController::trackDeleted, this, &MainWindow::DeleteTrackFromList);
    connect(playerController.get(), &PlayerController::trackLoaded, this, &MainWindow::addTrackToList);
    connect(playerController.get(), &PlayerController::setCurrentRow, this, &MainWindow::setCurrentRow);
    connect(playerController.get(), &PlayerController::playOrStopUI, this, &MainWindow::onPlayOrStopUI);

    playerController->loadTracks();
}

MainWindow::~MainWindow(){
    playerController->saveTracks();
    delete ui;
}

void MainWindow::on_playOrStopButton_clicked(){
    playerController->playOrStop();
}

void MainWindow::on_addButton_clicked(){
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите трек", "", "Аудио файлы (*.mp3 *.wav *.flac)");
    if (!filePath.isEmpty()) playerController->addTrack(filePath,getduration(filePath));
}

int MainWindow::getduration(QString filePath){
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
    playerController->playnext();
}

void MainWindow::on_prevButton_clicked(){
    playerController->playprev();
}

void MainWindow::on_horizontalSlider_sliderMoved(int position){
    playerController->getPlayer()->setPosition(position);
    int min = position / 60;
    int sec = position % 60;
    ui->time->setText(QString::asprintf("%d:%02d", min, sec));
}

void MainWindow::on_TrackLists_itemClicked(QListWidgetItem *item){
        int index = ui->TrackLists->row(item);
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
    ui->TrackLists->addItem(name);
}


void MainWindow::DeleteTrackFromList(int index) {
    if (index >= 0 && index < ui->TrackLists->count())
        ui->TrackLists->takeItem(index);
}
    
void MainWindow::setCurrentRow(int index) {
    if (index >= 0 && index < ui->TrackLists->count()) {
        ui->TrackLists->setCurrentRow(index);
    }
}

void MainWindow::onPlayOrStopUI(bool isPlaying) {
    if (isPlaying) {
        ui->playOrStopButton->setText("⏸");
    } else {
        ui->playOrStopButton->setText("▶");
    }
}