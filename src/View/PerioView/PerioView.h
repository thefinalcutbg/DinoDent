#pragma once

#include <QWidget>
#include "ui_PerioView.h"
#include "PerioView/PerioChartItem.h"
#include "PerioView/PerioGraphicsButton.h"
#include "PerioView/PerioScene.h"
#include "IPerioView.h"


class PerioView : public QWidget, public IPerioView
{
    Q_OBJECT

    PerioPresenter* presenter{ nullptr };

    PerioScene* perioScene[2];

    PerioChartItem* upperChart[2];
    PerioChartItem* lowerChart[2];

    QPushButton* m_tooth[32];
    PerioStateButton* m_mobi[32];
    FurcationWidget* m_furcation[32];
    PerioSpinBox* m_Rec[64];
    PerioSpinBox* m_Attach[64];
    PerioSpinBox* m_PD[192];
    PerioSpinBox* m_CAL[192];
    PerioSpinBox* m_GM[192];
    PerioButton* m_BOP[192];
    PerioGraphicsButton* m_FMBS[128];
    PerioGraphicsButton* m_FMPS[128];


    void paintEvent(QPaintEvent* event) override;
    void calcRecession(int GM_idx);
    void disableColumn(int toothIdx, bool disabled);
    double calculateCalAvg();
    void refreshChartMeasurment(int idx);

public:
    PerioView(QWidget *parent = Q_NULLPTR);

    void setPresenter(PerioPresenter* presenter) override;
    void setToothHint(const ToothPaintHint& hint) override;
    void setPerioStatus(const PerioStatus& perioStatus) override;
    PerioStatus getPerioStatus() override;
    

private:
    Ui::PerioViewClass ui;
};
