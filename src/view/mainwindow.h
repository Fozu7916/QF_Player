#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#ifdef _WIN32
#include "../integration/mediaosd.h"
#endif

#include "../player/player.h"
#include "../model/track.h"
#include "../controller/playercontroller.h"
#include <QListWidgetItem>
#include <QTimer>
#include <QMovie>

class DurationController; // Forward declaration для избежания циклических зависимостей

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void addTrackToList(const QString& name);
    void deleteTrackFromList(int index);
    void setCurrentRow(int index);
    void onPlayOrStopUI(bool isPlaying);
    void GetTime(int* time);
    
#ifdef _WIN32
protected:
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif
#endif
    
private slots:
    void on_playOrStopButton_clicked();
    void on_addButton_clicked();
    void on_TrackLists_itemClicked(QListWidgetItem *item);
    void on_deleteButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_horizontalSlider_sliderMoved(int position);
    void on_pushButton_clicked(bool checked);
signals:
    void trackReadyToAdd(const QString &filePath, int durationSec);
private:
    Ui::MainWindow *ui;
    QTimer *sliderTimer;
    std::unique_ptr<PlayerController> playerController;
    void updateSliderAndTimerForIndex(int index);
    QMovie* gifMovie;
    QThread* durationWorkerThread;
    DurationController* durationController;
#ifdef _WIN32
    MediaOsd* m_osd;
    void registerGlobalMediaHotkeys();
    void unregisterGlobalMediaHotkeys();
    enum { HOTKEY_ID_PLAYPAUSE = 0xA100, HOTKEY_ID_NEXT = 0xA101, HOTKEY_ID_PREV = 0xA102, HOTKEY_ID_STOP = 0xA103 };
#endif
};

#endif // MAINWINDOW_H



