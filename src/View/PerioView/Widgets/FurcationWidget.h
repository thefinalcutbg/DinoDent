#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "PerioStateButton.h"
//#include "ui_FurcationWidget.h"

class FurcationWidget : public QWidget
{
	Q_OBJECT

	PerioStateButton* leftButton;
	PerioStateButton* rightButton;
	PerioStateButton* downButton;
	QLabel* dummy;

	void paintEvent(QPaintEvent* event) override;

public:
	FurcationWidget(QWidget *parent = Q_NULLPTR);
	void setIndex(int toothIndex);
	~FurcationWidget();

private:
	//Ui::FurcationWidget ui;
};
