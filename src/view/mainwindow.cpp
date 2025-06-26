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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_player(new Player(this))
    , isPlayed(false)
    , sliderTimer(nullptr)
{
    ui->setupUi(this);
    changebackground(this,":/images/fone.jpg");
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50);
    ui->volume->setText(QString::number(50));
    m_player->setVolume(50);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value){
        ui->volume->setText(QString::number(value));
        m_player->setVolume(value);
    });
    loadTrackList();
}

MainWindow::~MainWindow()
{
    saveTrackList();
    delete ui;
}

void MainWindow::saveTrackList() {
    QFile file("tracks.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& track : TrackLists) {
            out << track.getPath() << ";" << track.getLength() << "\n";
        }
        file.close();
    }
}

void MainWindow::loadTrackList() {
    QFile file("tracks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                QStringList parts = line.split(";");
                QString path = parts.value(0);
                int length = parts.value(1).toInt();
                Track new_track(path, length);
                TrackLists.push_back(new_track);
                ui->TrackLists->addItem(QFileInfo(path).fileName());
            }
        }
        file.close();
    }
}

void MainWindow::on_playOrStopButton_clicked()
{
    if (currentTrackIndex < 0 || currentTrackIndex >= TrackLists.size()) return;
    if (!isPlayed) {
        ui->playOrStopButton->setText("⏸");
        m_player->play();
        isPlayed = true;
    } else {
        ui->playOrStopButton->setText("▶");
        m_player->pause();
        isPlayed = false;
    }
}

void MainWindow::on_addButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите трек", "", "Аудио файлы (*.mp3 *.wav *.flac)");
    if (!filePath.isEmpty()) {
        QMediaPlayer mediaPlayer;
        mediaPlayer.setSource(QUrl::fromLocalFile(filePath));
        QEventLoop loop;
        QObject::connect(&mediaPlayer, &QMediaPlayer::durationChanged, &loop, &QEventLoop::quit);
        mediaPlayer.play();
        loop.exec();
        qint64 durationMs = mediaPlayer.duration();
        mediaPlayer.stop();
        int durationSec = static_cast<int>(durationMs / 1000);
        Track new_track(filePath, durationSec);
        TrackLists.push_back(new_track);
        ui->TrackLists->addItem(QFileInfo(filePath).fileName());
    }
}


void MainWindow::on_deleteButton_clicked()
{
    m_player->pause();
    isPlayed = false;
    ui->TrackLists->takeItem(currentTrackIndex);
    TrackLists.erase(TrackLists.begin() + currentTrackIndex);
    currentTrackIndex = -1;
}

void MainWindow::on_nextButton_clicked(){
    if(currentTrackIndex + 1 <= TrackLists.size() - 1){
        m_player->pause();
        currentTrackIndex++;
        ui->TrackLists->setCurrentRow(currentTrackIndex);
        m_player->loadFile(TrackLists[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }else{
        currentTrackIndex = 0;
        ui->TrackLists->setCurrentRow(currentTrackIndex);
        m_player->loadFile(TrackLists[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }
}

void MainWindow::on_prevButton_clicked(){
    if(currentTrackIndex  - 1  >= 0){
        m_player->pause();
        currentTrackIndex--;
        ui->TrackLists->setCurrentRow(currentTrackIndex);
        m_player->loadFile(TrackLists[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }else{
        currentTrackIndex = TrackLists.size() - 1;
        ui->TrackLists->setCurrentRow(currentTrackIndex);
        m_player->loadFile(TrackLists[currentTrackIndex].getPath());
        m_player->play();
        isPlayed = true;
    }

}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    m_player->setPosition(position);
    int min = position / 60;
    int sec = position % 60;
    ui->time->setText(QString::asprintf("%d:%02d", min, sec));
}


void MainWindow::on_TrackLists_itemClicked(QListWidgetItem *item)
{
    int index = ui->TrackLists->row(item);
    if (index >= 0 && index < TrackLists.size()) {
        currentTrackIndex = index;
        m_player->loadFile(TrackLists[index].getPath());
        m_player->play();
        isPlayed = true;
        ui->horizontalSlider->setMaximum(TrackLists[index].getLength());
        if (!sliderTimer) {
            sliderTimer = new QTimer(this);
            connect(sliderTimer, &QTimer::timeout, this, [this]() {
                if (currentTrackIndex >= 0 && currentTrackIndex < TrackLists.size()) {
                    int pos = m_player->getPosition();
                    ui->horizontalSlider->setValue(pos);
                    int min = pos / 60;
                    int sec = pos % 60;
                    ui->time->setText(QString::asprintf("%d:%02d", min, sec));
                }
            });
        }
        sliderTimer->start(500);
    }

}

