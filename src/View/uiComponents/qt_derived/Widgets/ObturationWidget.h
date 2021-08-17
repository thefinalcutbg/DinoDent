#pragma once

#include <QWidget>
#include "ui_ObturationWidget.h"
#include "Model/StatusData.h"

class ObturationWidget : public QWidget
{
	Q_OBJECT

public:
	ObturationWidget(QWidget *parent = Q_NULLPTR);
	~ObturationWidget();

	ObturationData getData();
	void setData(const ObturationData& data);

private:
	Ui::ObturationWidget ui;
};
