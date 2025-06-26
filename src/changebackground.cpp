#include "changebackground.h"
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QSize>
#include <Qt>

void changebackground(QMainWindow *here,QString backgroundpath){
    QPixmap bkgnd(backgroundpath);
    QPixmap scaledBkgnd = bkgnd.scaled(here->size(), Qt::KeepAspectRatioByExpanding);
    QPalette pal;
    pal.setBrush(QPalette::Window, scaledBkgnd);
    here->setPalette(pal);
}