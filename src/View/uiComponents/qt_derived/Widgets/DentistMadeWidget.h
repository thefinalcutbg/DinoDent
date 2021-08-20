#pragma once

#include <QWidget>
#include "ui_DentistMadeWidget.h"
#include "Model/Tooth/StatusData.h"

class DentistMadeWidget : public QWidget
{
	Q_OBJECT

public:
	DentistMadeWidget(QWidget *parent = Q_NULLPTR);
	void setData(const DentistData& data);
	bool userChecked();
	~DentistMadeWidget();

private:
	Ui::DentistMadeWidget ui;
};
