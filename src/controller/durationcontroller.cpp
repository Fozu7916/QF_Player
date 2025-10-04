#include "durationcontroller.h"
#include <QtMultimedia/QMediaPlayer>
#include <QUrl>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

DurationController::DurationController(QObject *parent) : QObject(parent) {}

void DurationController::processFiles(const QStringList &filePaths) {
    QMediaPlayer mediaPlayer;
    for (const QString &filePath : filePaths) {
        if (filePath.isEmpty()) continue;
        mediaPlayer.setSource(QUrl::fromLocalFile(filePath));
        QEventLoop loop;
        QObject::connect(&mediaPlayer, &QMediaPlayer::durationChanged, &loop, &QEventLoop::quit);
        // Таймаут для предотвращения бесконечного ожидания
        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        mediaPlayer.play();
        loop.exec();
        mediaPlayer.stop();
        qint64 durationMs = mediaPlayer.duration();
        int durationSec = static_cast<int>(durationMs / 1000);
        qDebug() << "DurationController: Длительность для" << filePath << ":" << durationSec << "секунд";
        emit durationReady(filePath, durationSec);
    }
    emit finished();
}
