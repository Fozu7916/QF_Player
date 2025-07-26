#ifndef TRACK_H
#define TRACK_H

#include <QString>


class Track
{
public:
    Track();

    Track(QString path, int length);

    QString getPath() const { return path; }
    int getLength() const { return length; }

private:
    QString path;
    int length; // seconds

};

#endif // TRACK_H
