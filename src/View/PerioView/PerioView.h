#pragma once

#include <QWidget>
#include "ui_PerioView.h"

#include "StatisticTableModel.h"

#include "IPerioView.h"
#include "ToothUi.h"
#include "PerioGraphics/HexagonGraphicsItem.h"

class ToothButton;
class PerioChartItem;
class PerioGraphicsButton;
class PerioSpinBox;
class PerioScene;
class PerioStatistic;

enum ChartPosition { maxBuccal, maxPalatal, mandBuccal, mandLing };

struct ChartIndex
{
    ChartPosition position;
    int index;
};

class PerioView : public QWidget, public IPerioView
{
    Q_OBJECT


    PerioPresenter* presenter{ nullptr };

    PerioScene* perioScene[2];

    PerioChartItem* perioChart[4];

    ChartIndex chartIndex[192];

    const inline static PerioButtonStates m_mobiStates{"I", "II", "III"};

    ToothButton* m_tooth[32];
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

    StatisticTableModel stat_model;

    HexagonGraphicsItem hexagonGraphic;

    QRadioButton* m_smoke[5];

    void initializeCommon();
    void initializeSurfaces();
    void initializeFullMouth();
    void initializeRecAndAtt();
    void initializeTeethScenes();

    void paintEvent(QPaintEvent* event) override;

    void refreshChartMeasurment(int idx);

    ToothUi getUIbyTooth(int idx);

public:
    PerioView(QWidget *parent = Q_NULLPTR);

    void PerioGraphicClicked(int index, PerioGraphicsType type);

    void disableTooth(int index) override;
    void setToothData(const PerioToothData& data) override;
    void setPerioStatistic(const PerioStatistic& stat) override;
    void setPresenter(PerioPresenter* presenter) override;
    void setToothHint(const ToothPaintHint& hint) override;
    void setAdditional(int smoker, int boneLoss, bool systemic, bool restore) override;
    void setTeethView(ShowTeeth t) override;
    void setPatient(const Patient& patient, Date date) override;
    void setMeasurment(int index, int pd, int cal, int gm, int recession) override;
    

private:
    Ui::PerioViewClass ui;
};
