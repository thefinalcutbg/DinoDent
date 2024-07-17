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

void UnusedPackageView::setProgressCount(int count)
{
	ui.progressBar->setMaximum(count);
}

void UnusedPackageView::increment()
{
	ui.progressBar->setValue(ui.progressBar->value()+1);
}

UnusedPackageView::~UnusedPackageView()
{}
