#pragma once

#include <QDialog>
#include "ui_AmbListSelector.h"

class AmbListSelector : public QDialog
{
	Q_OBJECT

public:
	AmbListSelector(QWidget *parent = Q_NULLPTR);
	~AmbListSelector();

private:
	Ui::AmbListSelector ui;
};
