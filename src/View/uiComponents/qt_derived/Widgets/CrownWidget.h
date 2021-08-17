#pragma once

#include <QWidget>
#include "ui_CrownWidget.h"
#include "Model/StatusData.h"

class CrownWidget : public QWidget
{
	Q_OBJECT

public:
	CrownWidget(QWidget *parent = Q_NULLPTR);
	~CrownWidget();

	void setData(const CrownData& data);
	CrownData getData();

private:
	Ui::CrownWidget ui;
};
