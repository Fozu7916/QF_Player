#include "mediaosd.h"
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QScreen>

MediaOsd::MediaOsd(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    layoutUi();
    connect(&m_hideTimer, &QTimer::timeout, this, [this]() { hide(); });
}

void MediaOsd::layoutUi() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 16, 24, 16);
    root->setSpacing(8);

    auto *card = new QWidget(this);
    card->setObjectName("card");
    card->setStyleSheet("#card{ background: rgba(30,30,30,180); border-radius: 12px; }");
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0,0,0,120));
    card->setGraphicsEffect(shadow);

    auto *v = new QVBoxLayout(card);
    v->setContentsMargins(16, 12, 16, 12);
    v->setSpacing(6);

    m_icon = new QLabel(card);
    m_icon->setAlignment(Qt::AlignCenter);
    m_icon->setStyleSheet("font-size: 28pt; color: white;");
    m_text = new QLabel(card);
    m_text->setAlignment(Qt::AlignCenter);
    m_text->setStyleSheet("font-size: 12pt; color: white;");

    v->addWidget(m_icon);
    v->addWidget(m_text);
    root->addWidget(card);
}

void MediaOsd::showMessage(const QString& iconText, const QString& message, int ms) {
    m_icon->setText(iconText);
    m_text->setText(message);

    QRect r = QApplication::primaryScreen()->availableGeometry();
    resize(320, 120);
    move(r.center().x() - width()/2, r.center().y() - height()/2);
    show();
    raise();
    m_hideTimer.start(ms);
}


