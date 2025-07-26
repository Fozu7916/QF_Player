#include "changebackground.h"
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QSize>
#include <Qt>

void changebackground(QMainWindow *window,QString backgroundpath){
    QPixmap bkgnd(backgroundpath);
    QPixmap scaledBkgnd = bkgnd.scaled(window->size(), Qt::KeepAspectRatioByExpanding);
    QPalette pal;
    pal.setBrush(QPalette::Window, scaledBkgnd);
    window->setPalette(pal);
}