#ifndef MEDIAOSD_H
#define MEDIAOSD_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class MediaOsd : public QWidget {
    Q_OBJECT
public:
    explicit MediaOsd(QWidget* parent = nullptr);
    void showMessage(const QString& iconText, const QString& message, int ms = 1200);
private:
    QLabel* m_icon;
    QLabel* m_text;
    QTimer m_hideTimer;
    void layoutUi();
};

#endif // MEDIAOSD_H
