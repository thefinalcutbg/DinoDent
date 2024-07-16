#pragma once

#include <QWidget>
#include "ui_UnusedPackageView.h"
#include "Presenter/UnusedPackagePresenter.h"

class UnusedPackageView : public QWidget
{
	Q_OBJECT

	UnusedPackagePresenter presenter{ this };

public:
	UnusedPackageView(QWidget *parent = nullptr);
	void appendText(const std::string& line);
	~UnusedPackageView();

private:
	Ui::UnusedPackageViewClass ui;
};
