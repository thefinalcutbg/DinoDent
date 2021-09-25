#pragma once

#include <QWidget>
#include "ui_PerioView.h"
#include "PerioView/PerioChartItem.h"
#include "PerioView/PerioScene.h"
#include "IPerioView.h"

struct PerioDataUi
{
    QPushButton* tooth;
    PerioStateButton* mobi;
    FurcationWidget* furc;
    PerioSpinBox* rec[2];
    PerioSpinBox* attach[2];
    PerioSpinBox* PD[6];
    PerioSpinBox* CAL[6];
    PerioSpinBox* GM[6];
    PerioButton* BOP[6];

};

class PerioView : public QWidget, public IPerioView
{
    Q_OBJECT

    PerioPresenter* presenter{ nullptr };
    /*
    PerioChartItem* upperChart;
    PerioChartItem* lowerChart;
        */
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

    void addToLayout(
        PerioStatusView* view,
        PerioScene* &perioScene,
        PerioChartItem* &upperChart,
        PerioChartItem* &lowerChart,
        QPushButton* m_tooth[16],
        PerioStateButton* m_mobi[16],
        FurcationWidget* m_furcation[16],
        PerioSpinBox* m_Rec[32],
        PerioSpinBox* m_Attach[32],
        PerioSpinBox* m_PD[96],
        PerioSpinBox* m_CAL[96],
        PerioSpinBox* m_GM[96],
        PerioButton* m_BOP[96]
    );

    void paintEvent(QPaintEvent* event) override;
    void calcRecession(int GM_idx);
    void disableColumn(int toothIdx, bool disabled);

public:
    PerioView(QWidget *parent = Q_NULLPTR);

    void setPresenter(PerioPresenter* presenter) override;
    void setToothHint(const ToothPaintHint& hint) override;
    void setPerioStatus(const PerioStatus& perioStatus) override;
    PerioStatus getPerioStatus() override;
    

private:
    Ui::PerioViewClass ui;
};
