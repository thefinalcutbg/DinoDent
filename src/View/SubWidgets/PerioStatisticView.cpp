#include "PerioStatisticView.h"
#include "Model/Dental/PerioStatistic.h"
#include "View/GlobalFunctions.h"
#include "View/Graphics/HexagonGraphicsItem.h"
#include "View/Theme.h"
#include <QPainter>
constexpr std::string_view perioStage[5]{ "Здрав пародонт", "Начален" , "Умерен", "Тежък", "Напреднал" };
constexpr std::string_view perioRisk[3]{ "Нисък", "Среден", "Висок" };


void PerioStatisticView::paintEvent(QPaintEvent*)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.fillRect(rect(), Theme::background);

	QPainterPath path;

	path.addRoundedRect(
		QRectF(0, 0, width(), height()),
		Theme::radius / 2,
		Theme::radius / 2
	);

	painter.fillPath(path, Theme::sectionBackground);

	painter.setPen(QPen(Theme::border));
	painter.drawPath(path);

	painter.end();
}

PerioStatisticView::PerioStatisticView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	hexagonGraphicsItem = new HexagonGraphicsItem();

	ui.hexGraphicsView->setScene(new QGraphicsScene());
	ui.hexGraphicsView->scene()->addItem(hexagonGraphicsItem);
	ui.hexGraphicsView->setStyleSheet("background: transparent");

	setStyleSheet("QLabel { color :" + Theme::colorToString(Theme::fontTurquoise) + ";}");
}

void PerioStatisticView::setPerioStatistic(const PerioStatistic& stat)
{
	hexagonGraphicsItem->setRiskValue(stat.riskHexagon, static_cast<int>(stat.risk));

	ui.FMBS->setText(roundDouble(stat.BI) + " %");
	ui.FMPS->setText(roundDouble(stat.HI) + " %");
	ui.BOP->setText(roundDouble(stat.BOP) + " %");
	ui.boneLoss->setText(roundDouble(stat.boneIdx));
	ui.missingTeeth->setText(QString::number(stat.missingTeeth));

	ui.pdAvg->setText(roundDouble(stat.pdAverage) + " mm");

	ui.pd3->setText(QString::number(stat.pdHistogramCount[0]) + " (" +
					roundDouble(stat.pdHistogramPercentage[0]) + "%)");

	ui.pd45->setText(QString::number(stat.pdHistogramCount[1]) + " (" +
					roundDouble(stat.pdHistogramPercentage[1]) + "%)");

	ui.pd67->setText(QString::number(stat.pdHistogramCount[2]) + " (" +
					roundDouble(stat.pdHistogramPercentage[2]) + "%)");

	ui.pd7up->setText(QString::number(stat.pdHistogramCount[3]) + " (" +
					roundDouble(stat.pdHistogramPercentage[3]) + "%)");


	ui.calAvg->setText(roundDouble(stat.calAverage) + "mm");
	ui.calDist->setText(roundDouble(stat.calDistribution) + "%");

	ui.cal12->setText(QString::number(stat.calHistogramCount[0]) + " (" +
		roundDouble(stat.calHistogramCount[0]) + "%)");

	ui.cal34->setText(QString::number(stat.calHistogramCount[1]) + " (" +
		roundDouble(stat.calHistogramCount[1]) + "%)");

	ui.cal5up->setText(QString::number(stat.calHistogramCount[2]) + " (" +
		roundDouble(stat.calHistogramCount[2]) + "%)");


	QString diagnosis = perioStage[static_cast<int>(stat.stage)].data();

	if (stat.stage != Stage::Healthy)
	{
		diagnosis += stat.localized ? " локализиран" : " генерализиран";
		diagnosis += " периодонтит";
	}

	ui.diagnosis->setText("Диагноза: " + diagnosis);

	QString risk = "Оценка на риска: ";
	risk += perioRisk[static_cast<int>(stat.risk)].data();
	risk += " риск";

	ui.risk->setText(risk);
}

PerioStatisticView::~PerioStatisticView()
{}
