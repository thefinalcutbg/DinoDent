#pragma once

#include <QWidget>
#include "ui_UnusedPackageView.h"
#include "Presenter/UnusedPackagePresenter.h"

class UnusedPackageView : public QWidget
{
	Q_OBJECT

	UnusedPackagePresenter presenter;

	void exportToCSV();

public:

	UnusedPackageView(QWidget *parent = nullptr);
	void addRow(const PackageRowData& row);
	void addTable(const std::vector<PackageRowData>& table);
	void setProgressCount(int count);
	void reset();
	void increment();
	void setSumLabel(double price);
	~UnusedPackageView();

private:
	Ui::UnusedPackageViewClass ui;
};
