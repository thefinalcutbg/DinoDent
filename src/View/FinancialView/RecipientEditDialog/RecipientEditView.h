#pragma once

#include <QWidget>
#include "ui_RecipientEditView.h"

class RecipientEditView : public QWidget
{
	Q_OBJECT

public:
	RecipientEditView(QWidget *parent = nullptr);
	~RecipientEditView();

private:
	Ui::RecipientEditViewClass ui;
};
