#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "player.h"
#include "track.h"

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

private:
    Ui::MainWindow *ui;
    Player *m_player;
    bool isPlayed;
    std::vector<Track> TrackLists;
};

#endif // MAINWINDOW1_H



