#ifndef TRACK_H
#define TRACK_H

#include <QString>


class Track
{
public:
    Track();

    Track(QString path1, int length1, QString author1);

private:
    QString path;
    int length; // seconds
    QString author;

};

#endif // TRACK_H
