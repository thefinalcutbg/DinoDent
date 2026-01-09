// ShadowBakeWidget.h
#pragma once

#include <QWidget>
#include <QPixmap>
#include <vector>

class ShadowBakeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShadowBakeWidget(QWidget* parent = nullptr);

    void setShadowTargets(const std::vector<QWidget*>& targets);

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void childEvent(QChildEvent* e) override;
    bool event(QEvent* e) override;
    bool eventFilter(QObject* watched, QEvent* e) override;
        
private:
    void scheduleBake();
    void bakeNow();

    void setTargetsShadowEnabled(bool enabled);
    void installFiltersFor(QWidget* w);
    void removeDeadTargets();

private:
    std::vector<QWidget*> m_targets;
    QPixmap m_baked;

    bool m_bakeScheduled = false;
    int  m_debounceMs = 0;
};
