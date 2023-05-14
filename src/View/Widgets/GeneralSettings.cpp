#include "GeneralSettings.h"
#include <QPainter>

GeneralSettings::GeneralSettings(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void GeneralSettings::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::white);
}


GeneralSettings::~GeneralSettings()
{}
