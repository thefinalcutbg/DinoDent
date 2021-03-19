#pragma once

#include <QWidget>
#include "ui_RangeWidget.h"
#include "View/uiComponents/AbstractRangeEdit.h"

class RangeWidget : public QWidget, public AbstractRangeEdit
{
	Q_OBJECT

	Validator* validator;
	void stateChangedByUser() override;
public:
	RangeWidget(QWidget *parent = Q_NULLPTR);
	~RangeWidget();

	void disable(bool disable) override;
	void setRange(int begin, int end) override;
	std::tuple<int, int> getRange() override;
	void setAppearence(bool valid) override;
	void setFocusAndSelectAll() override;

signals:
	void rangeChanged(int begin, int end);
private:
	Ui::RangeWidget ui;
};
