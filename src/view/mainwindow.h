#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../player/player.h"
#include "../model/track.h"
#include "../controller/playercontroller.h"
#include <QListWidgetItem>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_playOrStopButton_clicked();
    void on_addButton_clicked();
    void on_TrackLists_itemClicked(QListWidgetItem *item);
    void on_deleteButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_horizontalSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    Player *m_player;
    bool isPlayed;
    int currentTrackIndex = -1;
    std::vector<Track> TrackLists;
    QTimer *sliderTimer;
    PlayerController* playerController;
    void saveTrackList();
    void loadTrackList();
};

#endif // MAINWINDOW1_H



