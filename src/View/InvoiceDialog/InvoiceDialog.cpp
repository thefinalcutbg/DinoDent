#include "InvoiceDialog.h"
#include <QFileDialog>
#include "Presenter/InvoicePresenter/InvoicePresenter.h"
#include "Model/Financial/Invoice.h"
#include "BusinessOperationModel.h"
#include "View/GlobalFunctions.h"

#include <QDebug>

InvoiceDialog::InvoiceDialog(InvoicePresenter* presenter, QWidget *parent) : 
	QDialog(parent),
	presenter(presenter)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Window);
	showMaximized();

	setWindowTitle(u8"Генериране на финансов документ по месечно известие от НЗОК");

	ui.invoiceDate->setDate(QDate::currentDate());

	ui.invoiceDate->setDisabled(true);
	ui.invoiceNumber->setDisabled(true);
	ui.printButton->setDisabled(true);
	ui.savePDFButton->setDisabled(true);
	ui.saveXMLButton->setDisabled(true);

	m_report.loadFromFile(":/reports/report_invoice.lrxml");
	
	//ui.graphicsView->scene()->scale

	connect(ui.loadButton, &QPushButton::clicked, [=]
		{
			QString filePath = QFileDialog::getOpenFileName(this,
				tr(u8"Изберете месечно известие"), "", tr("XML files(*.xml)"));

			if (filePath.isEmpty())
				return;
			
			//QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
			//QByteArray encodedString = codec->fromUnicode(filePath);
			//presenter->loadFile(encodedString.constData());

			presenter->loadFile(filePath.toStdString());

		});

	presenter->setView(this);

	connect(ui.saveXMLButton, &QPushButton::clicked, [=]
		{
			if (!presenter->invoiceIsValid()) {
				return;
			}

			QString dirPath = 
				QFileDialog::getExistingDirectory(this, tr("Open Directory"),
				"/home",
				QFileDialog::ShowDirsOnly
				| QFileDialog::DontResolveSymlinks);

			if (dirPath.isEmpty())
				return;

			presenter->saveAsXML(dirPath.toStdString());

		});

	connect(ui.invoiceNumber, QOverload<int>::of(&QSpinBox::valueChanged),
		[=](int i)
		{ 
			presenter->docNumberChanged(i);
		});

	connect(ui.invoiceDate, &QDateEdit::dateChanged,
		[=](QDate date)
		{
			presenter->docDateChanged(Date{ date.day(), date.month(), date.year() });
		});

	connect(ui.printButton, &QPushButton::pressed,
		[=] {
				if (!presenter->invoiceIsValid()) {
					return;
				}

				m_report.printReport();

		});

	connect(ui.savePDFButton, &QPushButton::pressed,
		[=] {
			if (!presenter->invoiceIsValid()) {
				return;
			}

			m_report.printToPDF(QFileDialog::getSaveFileName());

		});

}


void InvoiceDialog::enableUserFields()
{
	ui.invoiceDate->setDisabled(false);
	ui.invoiceNumber->setDisabled(false);
	ui.printButton->setDisabled(false);
	ui.savePDFButton->setDisabled(false);
	ui.saveXMLButton->setDisabled(false);

	ui.invoiceDate->setDate(QDate::currentDate());
	ui.invoiceNumber->setValue(0);

}

void InvoiceDialog::setInvoice(const Invoice& inv)
{
	m_report.dataManager()->setReportVariable("title", QString::fromStdString(inv.name));
	m_report.dataManager()->setReportVariable("number_date", QString::fromStdString(u8" № " + inv.number + u8" от дата " + inv.date.toString() + u8"г."));
	
	if (inv.mainDocument.has_value()) {
		m_report.dataManager()->setReportVariable(
			"main_document", 
			QString::fromStdString(
				u8"към фактура № " 
				+ inv.mainDocument.value().number 
				+ u8" от " + inv.mainDocument.value().date.toString() 
				+ u8"г."));
	}

	m_report.dataManager()->setReportVariable("recipient_name", QString::fromStdString(inv.recipient.name));
	m_report.dataManager()->setReportVariable("recipient_address", QString::fromStdString(inv.recipient.address));
	m_report.dataManager()->setReportVariable("recipient_bulstat", QString::fromStdString(inv.recipient.bulstat));
	
	m_report.dataManager()->setReportVariable("issuer_name", QString::fromStdString(inv.issuer.company_name));
	m_report.dataManager()->setReportVariable("issuer_address", QString::fromStdString(inv.issuer.address_by_contract));
	m_report.dataManager()->setReportVariable("issuer_bulstat", QString::fromStdString(inv.issuer.bulstat));

	m_report.dataManager()->setReportVariable("total", formatDoubleWithDecimal(inv.aggragated_amounts.total_amount));

	if (inv.nzokData.has_value())
	{
		m_report.dataManager()->setReportVariable("practice_rzi", QString::fromStdString(inv.nzokData->rhi_nhif_no));
		m_report.dataManager()->setReportVariable("contract", QString::fromStdString(inv.nzokData->contract_no + " / " + inv.nzokData->contract_date.toString()) + u8" г.");
		m_report.dataManager()->setReportVariable("mon_notif_number", QString::fromStdString(u8"Информация от месечно известие № " + inv.nzokData->fin_document_month_no));
		m_report.dataManager()->setReportVariable("period", QString::fromStdString(u8"от " + inv.nzokData->date_from.toString() + u8" до " + inv.nzokData->date_to.toString()));

	}
	
	BusinessOperationModel* model = new BusinessOperationModel{ inv.businessOperations };
	
	m_report.dataManager()->addModel("operations", model, true);

	auto scene = m_report.createPreviewScene();
	ui.graphicsView->setScene(scene);
	ui.graphicsView->fitInView(0, 0, 450, 1500, Qt::AspectRatioMode::KeepAspectRatio);
}

void InvoiceDialog::refreshNumberAndDate(const std::string& number, const Date& date)
{
	m_report.dataManager()->setReportVariable("number_date", QString::fromStdString(u8" № " + number + u8" от дата " + date.toString() + u8"г."));

	auto scene = m_report.createPreviewScene();
	ui.graphicsView->setScene(scene);
	ui.graphicsView->fitInView(0, 0, 450, 1500, Qt::AspectRatioMode::KeepAspectRatio);
}

InvoiceDialog::~InvoiceDialog()
{
}
