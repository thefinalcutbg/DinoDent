#pragma once

#include <QObject>
#include <QPointer>

class QScrollArea;
class QAbstractScrollArea;
class QPropertyAnimation;
class QWidget;
class QEvent;

class SmoothWheelScroll : public QObject
{
    Q_OBJECT
public:
    explicit SmoothWheelScroll(QScrollArea* area, QObject* parent = nullptr);
    ~SmoothWheelScroll() override;

    void setPixelsPerStep(int px);
    void setDurationRangeMs(int minMs, int maxMs);
    void setBaseDurationMs(int baseMs);
    void setDistanceDivisor(int divisor);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    static QAbstractScrollArea* nearestScrollArea(QWidget* w);

private:
    QPointer<QScrollArea> m_area;
    QPropertyAnimation* m_anim = nullptr;

    int m_target = 0;
    int m_pxPerStep = 60;

    int m_baseMs = 120;
    int m_minMs = 120;
    int m_maxMs = 420;
    int m_distDiv = 2;
};
