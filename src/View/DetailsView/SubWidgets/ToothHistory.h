#pragma once

#include <QWidget>
#include "ui_ToothHistory.h"
#include "ToothHistoryModel.h"

class ToothHistory : public QWidget
{
	Q_OBJECT

	ToothHistoryModel m_model;

public:
	ToothHistory(QWidget *parent = Q_NULLPTR);
	void setModel(const std::vector<DetailsSummary>& summary);
	~ToothHistory();

private:
	Ui::ToothHistory ui;
};
