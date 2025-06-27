#include "PerioSummary.h"
#include "View/Theme.h"
PerioSummary::PerioSummary(QWidget *parent)
	: QWidget(parent)
{
    ui.setupUi(this);
    
    ui.buccalView->setDisabled(true);
    ui.buccalView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.buccalView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui.lingualView->setDisabled(true);
    ui.lingualView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.lingualView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    buccalScene = new TeethBuccalScene();
    lingualScene = new TeethLingualScene();

    ui.buccalView->setScene(buccalScene);
    ui.lingualView->setScene(lingualScene);

    lingualScene->showPerio(true);
    buccalScene->showPerio(true);

    connect(ui.buccalRadio, &QRadioButton::toggled, [&] { ui.stackedWidget->setCurrentIndex(0); });
    connect(ui.lingualRadio, &QRadioButton::toggled, [&] { ui.stackedWidget->setCurrentIndex(1); });
    connect(ui.perioRadio, &QRadioButton::toggled, [&] { ui.stackedWidget->setCurrentIndex(2); });

    connect(ui.statusSlider, &QSlider::valueChanged, this, [&](int value) { displaySnapshotToView(value); });
    connect(ui.nextButton, &QPushButton::clicked, this, [&] { ui.statusSlider->setValue(ui.statusSlider->value() + 1); });
    connect(ui.prevButton, &QPushButton::clicked, this, [&] { ui.statusSlider->setValue(ui.statusSlider->value() - 1); });

    setStyleSheet("QLabel { color :" + Theme::colorToString(Theme::fontTurquoise) + ";}");

    ui.perioRadio->toggle();
    ui.stackedWidget->setCurrentIndex(2);
}

void PerioSummary::setSnapshots(const std::vector<PerioSnapshot>& snapshots)
{
    m_snapshots = snapshots;

    if (snapshots.empty()) {

        ui.statusSlider->blockSignals(true);
        ui.statusSlider->setRange(0, 0);
        ui.statusSlider->setValue(0);
        ui.statusSlider->blockSignals(false);

        return;
    }

    ui.statusSlider->setRange(0, m_snapshots.size() - 1);
    ui.statusSlider->setValue(m_snapshots.size() - 1);
    ui.statusSlider->setEnabled(m_snapshots.size() > 1);
    ui.nextButton->setEnabled(m_snapshots.size() > 1);
    ui.prevButton->setEnabled(m_snapshots.size() > 1);

    if (m_snapshots.size() == 1) {
        emit ui.statusSlider->valueChanged(0);
    }
}

void PerioSummary::paintEvent(QPaintEvent* e)
{
    if (ui.perioRadio->isChecked()) {
        QWidget::paintEvent(e);
        return;
    }

    QPainter p(this);
    p.fillRect(rect(), Qt::white);
}

void PerioSummary::displaySnapshotToView(int index)
{
    auto& perio = m_snapshots[index].perioStatus;
    
    buccalScene->setMeasurments(perio.pd, perio.cal);
    lingualScene->setMeasurments(perio.pd, perio.cal);
   
    QString label = QString("Дата на пародонталното измерване:<b> ") + perio.date.toBgStandard(true).c_str() + "</b>";

    ui.label->setText(label);

    for (auto& t : m_snapshots[index].toothStatus) {
        buccalScene->display(t);
        lingualScene->display(t);
    }

    ui.perioStatisticView->setPerioStatistic(m_snapshots[index].perioStatistic, index ? &m_snapshots[index-1].perioStatistic : nullptr);
}