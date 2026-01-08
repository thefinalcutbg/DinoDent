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

QString getComparisonStr(double value, double prevValue, const char* suffix, bool lowerIsBetter = true)
{
	if (value == prevValue) {
		return roundDouble(value) + suffix;
	}
	auto difference = value - prevValue;
	QString result;
	result += roundDouble(value) + suffix;
	if (lowerIsBetter == difference < 0) {
		result += "<b><span style = \"color:" + Theme::colorToString(Theme::fontTurquoise) + " ;\">";
	}
	else {
		result += "<b><span style = \"color:red;\">";
	}
	if (difference > 0) {
		result += " (+";
	}
	else {
		result += " (";
	}
	result += roundDouble(difference) + suffix + ")</span></b>";
	return result;
}

QString getComparisonStr(const PerioStatistic& stat, const PerioStatistic& prev, double PerioStatistic::* statMember, const char* suffix, bool lowerIsBetter)
{
	return getComparisonStr(stat.*statMember, prev.*statMember, suffix, lowerIsBetter); 
}

PerioStatisticView::PerioStatisticView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    Theme::applyShadow(this);
	hexagonGraphicsItem = new HexagonGraphicsItem();

	ui.hexGraphicsView->setScene(new QGraphicsScene());
	ui.hexGraphicsView->scene()->addItem(hexagonGraphicsItem);
	ui.hexGraphicsView->setStyleSheet("background: transparent");

	setStyleSheet("QLabel { color:" + Theme::colorToString(Theme::fontTurquoise) + ";}");
}

void PerioStatisticView::setPerioStatistic(const PerioStatistic& stat, const PerioStatistic* const prev)
{
	hexagonGraphicsItem->setRiskValue(stat.riskHexagon, static_cast<int>(stat.risk));

	auto statPrev = prev ? *prev : stat;

	ui.FMBS->setText(getComparisonStr(stat, statPrev, &PerioStatistic::BI, "%", true));
	ui.FMPS->setText(getComparisonStr(stat, statPrev, &PerioStatistic::HI, "%", false));
	ui.BOP->setText(getComparisonStr(stat, statPrev, &PerioStatistic::BOP, "%", true));
	ui.boneLoss->setText(getComparisonStr(stat, statPrev, &PerioStatistic::boneIdx, "", true));

	ui.missingTeeth->setText(QString::number(stat.missingTeeth));

	ui.missingTeeth->setText(getComparisonStr(stat.missingTeeth, statPrev.missingTeeth, "", true));

	ui.pdAvg->setText(getComparisonStr(stat, statPrev, &PerioStatistic::pdAverage, "mm", true));

	ui.pd3->setText(QString::number(stat.pdHistogramCount[0]) + " / " +
		getComparisonStr(stat.pdHistogramPercentage[0], statPrev.pdHistogramPercentage[0], "%", false));

	ui.pd45->setText(QString::number(stat.pdHistogramCount[1]) + " / " +
		getComparisonStr(stat.pdHistogramPercentage[1], statPrev.pdHistogramPercentage[1], "%", true));

	ui.pd67->setText(QString::number(stat.pdHistogramCount[2]) + " / " +
		getComparisonStr(stat.pdHistogramPercentage[2], statPrev.pdHistogramPercentage[2], "%", true));

	ui.pd7up->setText(QString::number(stat.pdHistogramCount[3]) + " / " +
		getComparisonStr(stat.pdHistogramPercentage[3], statPrev.pdHistogramPercentage[3], "%", true));

	ui.calAvg->setText(getComparisonStr(stat, statPrev, &PerioStatistic::calAverage, "mm", true));
	ui.calDist->setText(getComparisonStr(stat, statPrev, &PerioStatistic::calDistribution, "%", true));

	ui.cal12->setText(QString::number(stat.calHistogramCount[0]) + " / " +
		getComparisonStr(stat.calHistogramPercentage[0], statPrev.calHistogramPercentage[0], "%", false));

	ui.cal34->setText(QString::number(stat.calHistogramCount[1]) + " / " +
		getComparisonStr(stat.calHistogramPercentage[1], statPrev.calHistogramPercentage[1], "%", true));

	ui.cal5up->setText(QString::number(stat.calHistogramCount[2]) + " / " +
		getComparisonStr(stat.calHistogramPercentage[2], statPrev.calHistogramPercentage[2],  "%", true));

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
