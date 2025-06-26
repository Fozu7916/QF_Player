#ifndef TRACK_H
#define TRACK_H

#include <QString>


class Track
{
public:
    Track();

    Track(QString path1, int length1);

    QString getPath() const { return path; }
    int getLength() const { return length; }
    QString getAuthor() const { return author; }

private:
    QString path;
    int length; // seconds
    QString author;

};

#endif // TRACK_H
