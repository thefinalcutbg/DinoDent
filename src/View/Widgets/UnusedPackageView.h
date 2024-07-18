#pragma once

#include <QWidget>
#include "ui_UnusedPackageView.h"
#include "Presenter/UnusedPackagePresenter.h"

class UnusedPackageView : public QWidget
{
	Q_OBJECT

	UnusedPackagePresenter presenter{ this };

	void exportToCSV();

public:

	struct RowView {
		long long rowid;
		std::string patientName;
		std::string patientPhone;
		std::string maxDate;
		bool exam;
		int procedure_count;
		int procedure_max;
		std::string lowerDenture;
		std::string upperDenture;

	};

	UnusedPackageView(QWidget *parent = nullptr);
	void addRow(const RowView& row);
	void setProgressCount(int count);
	void reset();
	void increment();
	~UnusedPackageView();

private:
	Ui::UnusedPackageViewClass ui;
};
