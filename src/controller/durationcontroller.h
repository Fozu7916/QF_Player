#ifndef DURATIONCONTROLLER_H
#define DURATIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>

class DurationController : public QObject
{
    Q_OBJECT
public:
    explicit DurationController(QObject *parent = nullptr);
public slots:
    void processFiles(const QStringList &filePaths);
signals:
    void durationReady(const QString &filePath, int duration);
    void finished();
};

#endif // DURATIONCONTROLLER_H
