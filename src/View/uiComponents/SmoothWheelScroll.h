#pragma once

#include <QObject>
#include <QPointer>

class QScrollArea;
class QAbstractScrollArea;
class QPropertyAnimation;
class QTimer;
class QWidget;
class QEvent;

class SmoothWheelScroll : public QObject
{
    Q_OBJECT
public:
    explicit SmoothWheelScroll(QScrollArea* area, QObject* parent = nullptr);
    ~SmoothWheelScroll() override;

    void setPixelsPerWheelStep(int px);
    void setDurationRangeMs(int minMs, int maxMs);
    void setBaseDurationMs(int baseMs);
    void setDistanceDivisor(int divisor);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    static QAbstractScrollArea* nearestScrollArea(QWidget* w);

    void animateToTarget();
    void addToTarget(int delta);
    void stopArrowRepeat();
    void startArrowRepeat(int dir);

private:
    QPointer<QScrollArea> m_area;
    QPropertyAnimation* m_anim = nullptr;

    int m_target = 0;
    int m_pxPerWheelStep = 60;

    int m_baseMs = 140;
    int m_minMs = 120;
    int m_maxMs = 520;
    int m_distDiv = 3;

    QTimer* m_repeatTimer = nullptr;
    int m_repeatDir = 0;
};
