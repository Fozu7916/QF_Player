#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_player(new Player(this))
    , isPlayed(false)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_playOrStopButton_clicked()
{
    if( !isPlayed ){
        m_player->play();
        isPlayed = true;
    } else{
        m_player->pause();
        isPlayed = false;
    }
}

