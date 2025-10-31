#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "../controller/durationcontroller.h"
#include "../controller/playercontroller.h"
#include <QFileDialog>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QSettings>
#include <QThread>

const int DEFAULT_VOLUME = 50;
const QString PLAYLIST_FILENAME = "tracks.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sliderTimer(nullptr)
    , playerController(std::make_unique<PlayerController>())
    , durationWorkerThread(nullptr)
    , durationController(nullptr)
{
    ui->setupUi(this);
    if (statusBar()) statusBar()->hide();

    durationWorkerThread = new QThread(this);
    durationController = new DurationController();
    durationController->moveToThread(durationWorkerThread);

    QSettings settings("QF_Player", "QF_Player");
    int savedVolume = settings.value("audio/volume", DEFAULT_VOLUME).toInt();
    connect(ui->trackList, &QListWidget::itemClicked, this, &MainWindow::on_TrackLists_itemClicked);
    connect(playerController.get(), &PlayerController::trackDeleted, this, &MainWindow::deleteTrackFromList);
    connect(playerController.get(), &PlayerController::trackLoaded, this, &MainWindow::addTrackToList);
    connect(playerController.get(), &PlayerController::setCurrentRow, this, &MainWindow::setCurrentRow);
    connect(playerController.get(), &PlayerController::playOrStopUI, this, &MainWindow::onPlayOrStopUI);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value){
        ui->volume->setText(QString::number(value));
        playerController->setVolume(value);
    });
    connect(durationController, &DurationController::durationReady, this, [this](const QString &filePath, int duration) {
        playerController->addTrack(filePath, duration);
    });
    connect(durationController, &DurationController::finished, this, [this]() { durationWorkerThread->quit(); });
    connect(durationWorkerThread, &QThread::finished, durationController, &QObject::deleteLater);
    
    sliderTimer = new QTimer(this);
    connect(sliderTimer, &QTimer::timeout, this, [this]() {
        if (playerController->getCurrentIndex() >= 0 && playerController->getCurrentIndex() < playerController->getTracks().size()) {
            int pos = playerController->getPosition();
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
    if (!sliderTimer->isActive()) sliderTimer->start(500);



    //nekoarc
    gifMovie = new QMovie(":/images/necoarc.gif");
    ui->nekoArcLabel->setMovie(gifMovie);
    gifMovie->start();
    //nekoarc
    


    ui->volumeSlider->setRange(0, 100);
    playerController->loadTracks(PLAYLIST_FILENAME);
    int lastIndex = settings.value("player/lastIndex", -1).toInt();
    if (lastIndex >= 0 && lastIndex < static_cast<int>(playerController->getTracks().size())) {
        playerController->onItemClicked(lastIndex);
        updateSliderAndTimerForIndex(lastIndex);
        playerController->playOrStop();
    }
    ui->volumeSlider->setValue(savedVolume);
    ui->volume->setText(QString::number(savedVolume));
    durationWorkerThread->start();
#ifdef _WIN32
    m_osd = new MediaOsd(this);
    registerGlobalMediaHotkeys();
#endif
}

MainWindow::~MainWindow(){
    QSettings settings("QF_Player", "QF_Player");
    settings.setValue("audio/volume", ui->volumeSlider->value());
    settings.setValue("player/lastIndex", playerController->getCurrentIndex());

    if (durationWorkerThread && durationWorkerThread->isRunning()) {
        durationWorkerThread->quit();
        durationWorkerThread->wait(3000); // Для безопасности
    }

    delete gifMovie;
    playerController->saveTracks(PLAYLIST_FILENAME);
    delete ui;
}


void MainWindow::on_playOrStopButton_clicked(){
    playerController->playOrStop();
}

void MainWindow::on_addButton_clicked(){
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Выберите треки", "", "Аудио файлы (*.mp3 *.wav *.flac)");
    if (filePaths.isEmpty()) return;
    QMetaObject::invokeMethod(durationController, "processFiles", Qt::QueuedConnection, Q_ARG(QStringList, filePaths));
}

void MainWindow::on_deleteButton_clicked(){
    playerController->deleteTrack();
    updateSliderAndTimerForIndex(playerController->getCurrentIndex());
    onPlayOrStopUI(false);
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
    if (index < 0 || index >= static_cast<int>(playerController->getTracks().size())) { qInfo() << "MainWindow: early return, playerController or player null"; return; }
    onPlayOrStopUI(true);
    ui->horizontalSlider->setMaximum(playerController->getTracks()[index].getLength());
    ui->horizontalSlider->setValue(0);
    ui->time->setText("0:00");
}

void MainWindow::on_horizontalSlider_sliderMoved(int position){
    playerController->setPosition(position);
    int min = position / 60;
    int sec = position % 60;
    ui->time->setText(QString::asprintf("%d:%02d", min, sec));
}

void MainWindow::addTrackToList(const QString& name) {
    ui->trackList->addItem(name);
}

void MainWindow::deleteTrackFromList(int index) {
    if(index < 0 or index >= ui->trackList->count()) { qInfo() << "MainWindow: early return, invalid index" << index; return; }
    ui->trackList->takeItem(index);

}
    
void MainWindow::setCurrentRow(int index) {
    if (index < 0) { qInfo() << "MainWindow: early return, invalid index" << index; return; }
    ui->trackList->setCurrentRow(index);
}

void MainWindow::onPlayOrStopUI(bool isPlaying) {
    if (isPlaying) {
        ui->playOrStopButton->setText("⏸");
        gifMovie->start();
    } else {
        ui->playOrStopButton->setText("▶");
        gifMovie->stop();
    }
}

void MainWindow::on_pushButton_clicked(bool checked)
{
    playerController->setRandom(checked);
}

#ifdef _WIN32
#include <windows.h>

#ifndef APPCOMMAND_MEDIA_NEXTTRACK
#define APPCOMMAND_MEDIA_NEXTTRACK 11
#endif
#ifndef APPCOMMAND_MEDIA_PREVIOUSTRACK
#define APPCOMMAND_MEDIA_PREVIOUSTRACK 12
#endif
#ifndef APPCOMMAND_MEDIA_STOP
#define APPCOMMAND_MEDIA_STOP 13
#endif
#ifndef APPCOMMAND_MEDIA_PLAY_PAUSE
#define APPCOMMAND_MEDIA_PLAY_PAUSE 14
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#else
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
#endif
{
    Q_UNUSED(eventType);
    MSG* msg = static_cast<MSG*>(message);
    if (!msg) return false;

    if (msg->message == WM_HOTKEY) {
        switch (static_cast<int>(msg->wParam)) {
            case HOTKEY_ID_PLAYPAUSE:
                playerController->playOrStop();
                if (m_osd) m_osd->showMessage("⏯", "Play/Pause", 1000);
                break;
            case HOTKEY_ID_NEXT:
                playerController->playNext();
                updateSliderAndTimerForIndex(playerController->getCurrentIndex());
                if (m_osd) m_osd->showMessage("⏭", "Next", 1000);
                break;
            case HOTKEY_ID_PREV:
                playerController->playPrev();
                updateSliderAndTimerForIndex(playerController->getCurrentIndex());
                if (m_osd) m_osd->showMessage("⏮", "Previous", 1000);
                break;
            case HOTKEY_ID_STOP:
                if (playerController->getPlayed()) {
                    playerController->playOrStop();
                    if (m_osd) m_osd->showMessage("⏹", "Stop", 1000);
                }
                break;
        }
        if (result) *result = 1;
        return true;
    }

    if (msg->message == WM_APPCOMMAND) {
        const int cmd = GET_APPCOMMAND_LPARAM(msg->lParam);
        switch (cmd) {
            case APPCOMMAND_MEDIA_PLAY_PAUSE:
                playerController->playOrStop();
                if (m_osd) m_osd->showMessage("⏯", "Play/Pause", 1000);
                break;
            case APPCOMMAND_MEDIA_NEXTTRACK:
                playerController->playNext();
                updateSliderAndTimerForIndex(playerController->getCurrentIndex());
                if (m_osd) m_osd->showMessage("⏭", "Next", 1000);
                break;
            case APPCOMMAND_MEDIA_PREVIOUSTRACK:
                playerController->playPrev();
                updateSliderAndTimerForIndex(playerController->getCurrentIndex());
                if (m_osd) m_osd->showMessage("⏮", "Previous", 1000);
                break;
            case APPCOMMAND_MEDIA_STOP:
                if (playerController->getPlayed()) {
                    playerController->playOrStop();
                    if (m_osd) m_osd->showMessage("⏹", "Stop", 1000);
                }
                break;
            default:
                break;
        }
        if (result) *result = 1;
        return true;
    }
    return false;
}
#endif

#ifdef _WIN32
void MainWindow::registerGlobalMediaHotkeys() {
    HWND hwnd = reinterpret_cast<HWND>(winId());
    RegisterHotKey(hwnd, HOTKEY_ID_PLAYPAUSE, 0, VK_MEDIA_PLAY_PAUSE);
    RegisterHotKey(hwnd, HOTKEY_ID_NEXT, 0, VK_MEDIA_NEXT_TRACK);
    RegisterHotKey(hwnd, HOTKEY_ID_PREV, 0, VK_MEDIA_PREV_TRACK);
    RegisterHotKey(hwnd, HOTKEY_ID_STOP, 0, VK_MEDIA_STOP);
}

void MainWindow::unregisterGlobalMediaHotkeys() {
    HWND hwnd = reinterpret_cast<HWND>(winId());
    UnregisterHotKey(hwnd, HOTKEY_ID_PLAYPAUSE);
    UnregisterHotKey(hwnd, HOTKEY_ID_NEXT);
    UnregisterHotKey(hwnd, HOTKEY_ID_PREV);
    UnregisterHotKey(hwnd, HOTKEY_ID_STOP);
}
#endif
