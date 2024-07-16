#include "UnusedPackageView.h"

UnusedPackageView::UnusedPackageView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	auto date = Date::currentDate();
	date.year--;
	date.day = 1;
	date.month = 1;

	ui.dateEdit->set_Date(date);

	connect(ui.generateButton, &QPushButton::clicked, this, [&] {
		presenter.generateReport(ui.dateEdit->getDate());
	});
}

void UnusedPackageView::appendText(const std::string& line)
{
	ui.textEdit->appendPlainText(line.c_str());
}

UnusedPackageView::~UnusedPackageView()
{}
