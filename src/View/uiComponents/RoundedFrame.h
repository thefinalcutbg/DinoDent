#pragma once
#include <QFrame>
#include <QPainter>

class RoundedFrame : public QFrame
{
    Q_OBJECT

    void paintEvent(QPaintEvent* event) override;

    int m_separatorPosition = -1;
    QColor m_color;
public:
    explicit RoundedFrame(QWidget* parent = nullptr);
    void setFrameColor(const QColor& c) {m_color = c; update();};
    void addVerticalSeparator(int x){ m_separatorPosition = x; update(); };

};
