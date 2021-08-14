#pragma once

#include <QWidget>
#include "ui_DetailedStatus.h"

class CheckModel;

class DetailedStatus : public QWidget
{
	Q_OBJECT

	bool m_checkChange{ false };

public:
	DetailedStatus(QWidget *parent = Q_NULLPTR);

	void setCheckModel(const CheckModel& checkModel);

	~DetailedStatus();

signals:
	void selectionChanged(int category, int code);
	void itemChecked();

private:
	Ui::DetailedStatus ui;




};
