#pragma once
#include <QFrame>
#include <QPainter>

class RoundedFrame : public QFrame
{
    Q_OBJECT

    void paintEvent(QPaintEvent* event) override;
    bool m_hasChildFocus = true;
    int m_separatorPosition = -1;
    QColor m_color;

    bool eventFilter(QObject* watched, QEvent* event) override;
    void updateFocusState();
public:
    explicit RoundedFrame(QWidget* parent = nullptr);
    void setFrameColor(const QColor& c) {m_color = c; update();};
    void setDynamicFocusBorderChange();
    void addVerticalSeparator(int x){ m_separatorPosition = x; update(); };

};
