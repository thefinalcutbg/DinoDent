#include "SnapshotViewer.h"
#include <QPainter>

SnapshotViewer::SnapshotViewer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	teeth_scene = new TeethViewScene();
    teeth_scene->drawFocused(true);
	ui.graphicsView->setScene(teeth_scene);
    ui.graphicsView->disableMultiSelection();

	ToothContainer empty_container;

	for (int i = 0; i < 32; i++)
	{
		teeth_scene->display(ToothPaintHint(empty_container[i]));
	}

	connect(ui.statusSlider, &QSlider::valueChanged, this, [&](int value) { displaySnapshotToView(m_snapshots[value]);});
    connect(ui.nextButton, &QPushButton::clicked, this, [&] { ui.statusSlider->setValue(ui.statusSlider->value()+1); });
    connect(ui.prevButton, &QPushButton::clicked, this, [&] { ui.statusSlider->setValue(ui.statusSlider->value()-1); });
}

void SnapshotViewer::setSnapshots(const std::vector<HisSnapshot>& snapshots)
{
	m_snapshots = snapshots;

    if (snapshots.empty()) {

        displaySnapshotToView(HisSnapshot());
        ui.statusSlider->blockSignals(true);
        ui.statusSlider->setRange(0, 0);
        ui.statusSlider->setValue(0);
        ui.statusSlider->blockSignals(false);

        return;
    }

	ui.statusSlider->setRange(0, m_snapshots.size()-1);
    ui.statusSlider->setValue(m_snapshots.size()-1);



    if (m_snapshots.size() == 1) {
        emit ui.statusSlider->valueChanged(0);
    }
}

const HisSnapshot* SnapshotViewer::getCurrentSnapshot() const
{
    if (m_snapshots.empty()) return nullptr;
    
    return &m_snapshots[ui.statusSlider->value()];
}

bool SnapshotViewer::isMostRecent() const
{
    if (m_snapshots.empty()) return true;

    return ui.statusSlider->value() == m_snapshots.size() - 1;
}

void SnapshotViewer::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
}

void SnapshotViewer::displaySnapshotToView(const HisSnapshot& s)
{
    for (int i = 0; i < 32; i++)
    {
        teeth_scene->display(ToothPaintHint(s.teeth[i]));
    }

    teeth_scene->setProcedures(s.affected_teeth);

    ui.nextButton->setDisabled(false);
    ui.prevButton->setDisabled(false);
    ui.statusSlider->setDisabled(false);

    

    ui.dateLabel->setText(
        s.date.isDefault() ? "" :
        QString("<b>Дата:</b> ") + s.date.toBgStandard().c_str());

    ui.procedureLabel->setText(
        s.procedure_name.empty() ? "" : 
        QString("<b>Процедура:</b> ") + s.procedure_name.c_str()
    );

    ui.diagnosisLabel->setText(
        s.procedure_diagnosis.empty() ? "" :
        QString("<b>Диагноза:</b> ") + s.procedure_diagnosis.c_str()
    );
    

    switch (s.financing)
    {
        case FinancingSource::NHIF: ui.financingLabel->setText("<b>Финансиране:</b> НЗОК"); break;
        case FinancingSource::Patient: ui.financingLabel->setText("<b>Финансиране:</b> Пациент"); break;
        case FinancingSource::PHIF: ui.financingLabel->setText("<b>Финансиране:</b> ДЗОФ"); break;
        case FinancingSource::University: ui.financingLabel->setText("<b>Финансиране:</b> МОН"); break;
        case FinancingSource::None: ui.financingLabel->setText("<b>Финансиране:</b> Без финансиране"); break;
        default: ui.financingLabel->clear();
    }

    if (s.date.isDefault()) {
        ui.financingLabel->clear();
    }

    if (s.procedure_note.size()) {
        ui.notesLabel->setText(QString("<b>Бележки:</b> ") + s.procedure_note.c_str());
    }
    else {
        ui.notesLabel->clear();
    }

}

SnapshotViewer::~SnapshotViewer()
{}
