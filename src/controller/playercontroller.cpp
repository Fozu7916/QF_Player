#include "playercontroller.cpp"

PlayerController::PlayerController(){};

PlayerController::loadTracks(){
    QFile file("tracks.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                QStringList parts = line.split(";");
                QString path = parts.value(0);
                int length = parts.value(1).toInt();
                Track new_track(path, length);
                TrackLists.push_back(new_track);
                ui->TrackLists->addItem(QFileInfo(path).fileName());
            }
        }
        file.close();
    }
}

PlayerController::saveTracks(){
    QFile file("tracks.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const auto& track : TrackLists) {
            out << track.getPath() << ";" << track.getLength() << "\n";
        }
        file.close();
    }
}