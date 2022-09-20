#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "PerioStateButton.h"
#include <string_view>

struct FurcationMeasurment { int a{ 0 }, b{ 0 }, c{ 0 }; };

class FurcationWidget : public QWidget
{
	Q_OBJECT

	PerioStateButton* leftButton;
	PerioStateButton* rightButton;
	PerioStateButton* downButton;
	QLabel* dummy;

	const inline static PerioButtonStates states{"F1", "F2", "F3"};

	void paintEvent(QPaintEvent* event) override;
	FurcationMeasurment getMeasurment();

public:
	FurcationWidget(QWidget *parent = Q_NULLPTR);
	
	void setMeasurment(int a, int b, int c);
	void setIndex(int toothIndex);
	~FurcationWidget();

signals:
	void valueChanged(FurcationMeasurment value);


private:
	//Ui::FurcationWidget ui;
};
