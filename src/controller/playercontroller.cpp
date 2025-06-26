#include "playercontroller.h"
#include <QFile>
#include <QTextStream>

PlayerController::PlayerController(){};

void PlayerController::loadTracks() {
    QFile file("tracks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        m_tracks.clear();
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                QStringList parts = line.split(";");
                QString path = parts.value(0);
                int length = parts.value(1).toInt();
                Track track(path, length);
                m_tracks.push_back(track);
            }
        }
        file.close();
    }
}

void PlayerController::saveTracks() {
    QFile file("tracks.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& track : m_tracks) {
            out << track.getPath() << ";" << track.getLength() << "\n";
        }
        file.close();
    }
}