#pragma once

#include <QWidget>
#include "ui_PerioView.h"
#include "PerioView/PerioChartItem.h"
#include "PerioView/PerioScene.h"
#include "IPerioView.h"



class PerioView : public QWidget, public IPerioView
{
    Q_OBJECT

    PerioPresenter* presenter{ nullptr };
    /*
    PerioChartItem* upperChart;
    PerioChartItem* lowerChart;

    QPushButton* m_tooth[32];
    PerioStateButton* m_mobi[32];
    FurcationWidget* m_furcation[32];
    PerioSpinBox* m_Rec[64];
    PerioSpinBox* m_Attach[64];
    PerioSpinBox* m_PD[192];
    PerioSpinBox* m_CAL[192];
    PerioSpinBox* m_GM[192];
    PerioButton* m_BOP[192];
    */
    void paintEvent(QPaintEvent* event) override;

public:
    PerioView(QWidget *parent = Q_NULLPTR);

    void setPresenter(PerioPresenter* presenter) override;
    void setToothHint(const ToothPaintHint& hint) override;
    void setPerioStatus(const PerioStatus& upper, const PerioStatus& lower) override;
    std::tuple<PerioStatus, PerioStatus> getPerioStatus() override;
    

private:
    Ui::PerioViewClass ui;
};
