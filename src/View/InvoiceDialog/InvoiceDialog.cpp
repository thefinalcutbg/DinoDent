#include "InvoiceDialog.h"
#include <QFileDialog>
#include "Presenter/InvoicePresenter/InvoicePresenter.h"
InvoiceDialog::InvoiceDialog(InvoicePresenter* presenter, QWidget *parent) : 
	QDialog(parent),
	presenter(presenter)
{
	ui.setupUi(this);

	ui.invoiceDate->setDate(QDate::currentDate());

	ui.invoiceDate->setDisabled(true);
	ui.invoiceNumber->setDisabled(true);
	ui.printButton->setDisabled(true);
	ui.savePDFButton->setDisabled(true);
	ui.saveXMLButton->setDisabled(true);

	connect(ui.loadButton, &QPushButton::clicked, [=]
		{
			QString filePath = QFileDialog::getOpenFileName(this,
				tr(u8"Изберете месечно известие"), "", tr("XML files(*.xml)"));

			if (filePath.isEmpty())
				return;

			presenter->loadFile(filePath.toStdString());

		});

	presenter->setView(this);

	connect(ui.saveXMLButton, &QPushButton::clicked, [=]
		{
			QString dirPath = 
				QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				"/home",
				QFileDialog::ShowDirsOnly
				| QFileDialog::DontResolveSymlinks);

			if (dirPath.isEmpty())
				return;

			presenter->saveAsXML(dirPath.toStdString());

		});
	
}

void InvoiceDialog::enableUserFields()
{
	ui.invoiceDate->setDisabled(false);
	ui.invoiceNumber->setDisabled(false);
	ui.printButton->setDisabled(false);
	ui.savePDFButton->setDisabled(false);
	ui.saveXMLButton->setDisabled(false);

}

InvoiceDialog::~InvoiceDialog()
{
}
