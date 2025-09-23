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
#include <QUrl>
#include <QSettings>
#include <QtConcurrent>
#include <QFuture>
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

    connect(ui->trackList, &QListWidget::itemClicked,
            this, &MainWindow::on_TrackLists_itemClicked);

    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value){
        ui->volume->setText(QString::number(value));
        playerController->setVolume(value);
    });
    connect(playerController.get(), &PlayerController::trackDeleted, this, &MainWindow::deleteTrackFromList);
    connect(playerController.get(), &PlayerController::trackLoaded, this, &MainWindow::addTrackToList);
    connect(playerController.get(), &PlayerController::setCurrentRow, this, &MainWindow::setCurrentRow);
    connect(playerController.get(), &PlayerController::playOrStopUI, this, &MainWindow::onPlayOrStopUI);

    //nekoarc
    gifMovie = new QMovie(":/images/necoarc.gif");
    ui->nekoArcLabel->setMovie(gifMovie);
    gifMovie->start();
    //nekoarc

    QSettings settings("QF_Player", "QF_Player");
    int savedVolume = settings.value("audio/volume", DEFAULT_VOLUME).toInt();

    ui->volumeSlider->setRange(0, 100);
    playerController->loadTracks(PLAYLIST_FILENAME);
    int lastIndex = settings.value("player/lastIndex", -1).toInt();
    if (lastIndex >= 0 && lastIndex < static_cast<int>(playerController->getTracks().size())) {
        playerController->setCurrentIndex(lastIndex);
        ui->horizontalSlider->setMaximum(playerController->getTracks()[lastIndex].getLength());
        ui->horizontalSlider->setValue(0);
        ui->time->setText("0:00");
        ui->trackList->setCurrentRow(lastIndex);
    }

    ui->volumeSlider->setValue(savedVolume);
    ui->volume->setText(QString::number(savedVolume));

}

MainWindow::~MainWindow(){
    QSettings settings("QF_Player", "QF_Player");
    settings.setValue("audio/volume", ui->volumeSlider->value());
    settings.setValue("player/lastIndex", playerController->getCurrentIndex());

    playerController->saveTracks(PLAYLIST_FILENAME);
    delete ui;
}

void MainWindow::on_playOrStopButton_clicked(){
    playerController->playOrStop();
}

void MainWindow::on_addButton_clicked(){
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Выберите треки", "", "Аудио файлы (*.mp3 *.wav *.flac)");
    for (const QString &filePath : filePaths) {
        if (filePath.isEmpty()) continue;
        QtConcurrent::run([this, filePath]() {
            int dur = getDuration(filePath);
            QMetaObject::invokeMethod(this, [this, filePath, dur]() {
                playerController->addTrack(filePath, dur);
            }, Qt::QueuedConnection);
        });
    }
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
    updateSliderAndTimerForIndex(playerController->getCurrentIndex());
}

void MainWindow::on_prevButton_clicked(){
    playerController->playPrev();
    updateSliderAndTimerForIndex(playerController->getCurrentIndex());
}

void MainWindow::on_TrackLists_itemClicked(QListWidgetItem *item){
        int index = ui->trackList->row(item);
        playerController->onItemClicked(index);
        updateSliderAndTimerForIndex(index);
}

void MainWindow::updateSliderAndTimerForIndex(int index) {
    if (index < 0 || index >= static_cast<int>(playerController->getTracks().size())) return;
    onPlayOrStopUI(true);
    ui->horizontalSlider->setMaximum(playerController->getTracks()[index].getLength());
    ui->horizontalSlider->setValue(0);
    ui->time->setText("0:00");
    if (!sliderTimer) {
        sliderTimer = new QTimer(this);
        connect(sliderTimer, &QTimer::timeout, this, [this]() {
            if (playerController->getCurrentIndex() >= 0 && playerController->getCurrentIndex() < playerController->getTracks().size()) {
                int pos = playerController->getPlayer()->getPosition();
                if (playerController->getPlayer()->isEof() || (ui->horizontalSlider->maximum() > 0 && pos >= ui->horizontalSlider->maximum() - 1)) {
                    playerController->playNext();
                    updateSliderAndTimerForIndex(playerController->getCurrentIndex());
                    return;
                }
                ui->horizontalSlider->setValue(pos);
                int min = pos / 60;
                int sec = pos % 60;
                ui->time->setText(QString::asprintf("%d:%02d", min, sec));
            }
        });
    }
    if (!sliderTimer->isActive()) sliderTimer->start(500);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position){
    playerController->getPlayer()->setPosition(position);
    int min = position / 60;
    int sec = position % 60;
    ui->time->setText(QString::asprintf("%d:%02d", min, sec));
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

void MainWindow::on_pushButton_clicked(bool checked)
{
    playerController->setRandom(checked);
}

