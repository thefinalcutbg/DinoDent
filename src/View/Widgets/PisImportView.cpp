#include "PisImportView.h"
#include "Model/Date.h"


PisImportView::PisImportView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	int currentYear = Date::currentYear();

	ui.toSpin->setValue(currentYear);

	ui.fromSpin->setMaximum(currentYear);
	ui.toSpin->setMaximum(currentYear);

	ui.importButton->setIcon(QIcon(":/icons/icon_nhif.png"));
	ui.xmlButton->setIcon(QIcon(":/icons/icon_xml.png"));

    connect(ui.toSpin, &QSpinBox::valueChanged, this, [&](int value) { ui.fromSpin->setMaximum(value); });

    connect(ui.importButton, &QPushButton::clicked, this, [&] { presenter.importData(ui.fromSpin->value(), ui.toSpin->value());});

	connect(ui.xmlButton, &QPushButton::clicked, this, [&] { presenter.loadXml(); });

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
	ui.label->setDisabled(disabled);
	ui.fromSpin->setDisabled(disabled);
	ui.label_2->setDisabled(disabled);
	ui.toSpin->setDisabled(disabled);
}

PisImportView::~PisImportView()
{}
