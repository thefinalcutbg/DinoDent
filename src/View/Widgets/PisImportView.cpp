#include "PisImportView.h"

PisImportView::PisImportView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.importButton->setIcon(QIcon(":/icons/icon_nhif.png"));

	connect(ui.importButton, &QPushButton::clicked, [&] { presenter.importDataPressed();});

	presenter.setView(this);

}

void PisImportView::setProgresSize(int size)
{
	ui.progressBar->setRange(0, size);
	ui.progressBar->setValue(0);
}

void PisImportView::incrementProgress()
{
	ui.progressBar->setValue(ui.progressBar->value() + 1);
}

void PisImportView::clearConsole()
{
	ui.textEdit->clear();
}

void PisImportView::logToConsole(const std::string& text)
{
	ui.textEdit->appendPlainText(text.c_str());
}

void PisImportView::disableButton(bool disabled)
{
	ui.importButton->setDisabled(disabled);
}

PisImportView::~PisImportView()
{}
