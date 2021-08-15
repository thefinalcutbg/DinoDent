#pragma once

#include <QWidget>
#include "ui_CrownWidget.h"

class CrownWidget : public QWidget
{
	Q_OBJECT

public:
	CrownWidget(QWidget *parent = Q_NULLPTR);
	~CrownWidget();

private:
	Ui::CrownWidget ui;
};
