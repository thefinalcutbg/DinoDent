#pragma once

#include <QWidget>
#include "ui_RangeWidget.h"
#include "View/Interfaces/AbstractRangeEdit.h"

class RangeWidget : public QWidget, public AbstractRangeEdit
{
	Q_OBJECT

private:
	Validator* validator;
	QLabel* errorLabel;

public:
	RangeWidget(QWidget *parent = Q_NULLPTR);
	~RangeWidget();

	void disable(bool disable) override;
	void setBridgeRange(int begin, int end) override;
	std::tuple<int, int> getRange() override;
	void setValidAppearence(bool valid) override;
	void setFocus() override;

	void setErrorLabel(QLabel* errorLabel);

signals:
	void rangeChanged(int begin, int end);

private:
	Ui::RangeWidget ui;
};