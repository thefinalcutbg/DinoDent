#include <QFileDialog>

#include "FinancialView.h"
#include "Model/Financial/Invoice.h"
#include "View/Theme.h"
#include "View/GlobalFunctions.h"
#include "View/uiComponents/MouseWheelGuard.h"
#include "QtVersion.h"

FinancialView::FinancialView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.numberSpinBox->setTotalLength(10);

	ui.numberSpinBox->installEventFilter(new MouseWheelGuard(ui.numberSpinBox));

	ui.saveXMLButton->setIcon(QIcon(":/icons/icon_xml.png"));
	ui.sendPisButton->setIcon(QIcon(":/icons/icon_nhif.png"));
	ui.addButton->setIcon(QIcon(":/icons/icon_add.png"));
	ui.deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.editButton->setIcon(QIcon(":/icons/icon_edit.png"));

	ui.operationsTable->setModel(&m_model);
	ui.operationsTable->setBusinessOperationLayout();
	ui.operationsTable->setStyleSheet(
		"color :" + Theme::colorToString(Theme::fontTurquoise) + "; "
		"selection-color:" + Theme::colorToString(Theme::fontTurquoiseClicked) + "; "
		"selection-background-color: " + Theme::colorToString(Theme::background) + "; "
	);

	setStyleSheet(Theme::getFancyStylesheet());

	ui.operationsLabel->setStyleSheet(
		"color : " + Theme::colorToString(Theme::fontTurquoise) + "; "
		 "font-weight: bold; font-size: 12px;"
	);


	ui.mainDocNumSpin->setTotalLength(10);

	connect(ui.recipientButton, &QPushButton::clicked, [=] {presenter->editRecipient();});

	connect(ui.dateEdit, &QDateEdit::dateChanged, 
		[=](QDate date){
			if (presenter == nullptr) return;
			presenter->dateChanged(Date(date.day(), date.month(), date.year()));
		});

	connect(ui.taxEventDateEdit, &QDateEdit::dateChanged,
		[=](QDate date) {
			if (presenter == nullptr) return;
			presenter->taxEventDateChanged(Date(date.day(), date.month(), date.year()));
		});

	connect(ui.paymentTypeCombo, QtComboIndexChanged,
		[=](int index) {
			presenter->paymentTypeChanged(static_cast<PaymentType>(index));
		});

	connect(ui.saveXMLButton, &QPushButton::clicked, [=] {presenter->saveAsXML(); });
	connect(ui.sendPisButton, &QPushButton::clicked, [=] {presenter->sendToPis();});

	connect(ui.numberSpinBox, &LeadingZeroSpinBox::valueChanged, [=](long long num) {if (presenter)presenter->invoiceNumberChanged(num);});

	connect(ui.deleteButton, &QPushButton::clicked,
		[=] {

			if (!presenter) return;

			int currentIdx = ui.operationsTable->selectedRow();
			int lastIdx = ui.operationsTable->verticalHeader()->count() - 1;

			presenter->removeOperation(currentIdx);
			
			if (currentIdx == lastIdx)
			{
				ui.operationsTable->selectRow(currentIdx - 1);
			}
			else ui.operationsTable->selectRow(currentIdx);
			
		});


	connect(ui.editButton, &QPushButton::clicked, [=]{ if (presenter) presenter->editOperation(ui.operationsTable->selectedRow());});

	connect(ui.addButton, &QAbstractButton::clicked, [=] { if (presenter) presenter->addOperation(); });


	connect(ui.operationsTable, &TableView::deletePressed,[=] { ui.deleteButton->click(); });
	connect(ui.operationsTable, &QTableView::doubleClicked, [=] { ui.editButton->click(); });
	connect(ui.docTypeCombo, QtComboIndexChanged, [=](int idx) { presenter->docTypeChanged(idx);});
	
	
	
	
	connect(ui.mainDocDateEdit, &QDateEdit::dateChanged, [=] (QDate d) {
		presenter->mainDocumentChanged(ui.mainDocNumSpin->value(), Date(d.day(), d.month(), d.year()));
		});
	connect(ui.mainDocNumSpin, &LeadingZeroSpinBox::valueChanged, [=](long long value) {
		auto d = ui.mainDocDateEdit->date();
		presenter->mainDocumentChanged(value, Date(d.day(), d.month(), d.year()));
		});
}

FinancialView::~FinancialView()
{
}

void FinancialView::setPresenter(FinancialPresenter* presenter)
{
	this->presenter = presenter;
}

void FinancialView::setInvoice(const Invoice& inv)
{
	ui.issuerButton->setIssuer(inv.issuer);
	ui.recipientButton->setRecipient(inv.recipient);

	QSignalBlocker blockers[4]{
		QSignalBlocker{ui.dateEdit},
		QSignalBlocker{ui.taxEventDateEdit},
		QSignalBlocker{ui.paymentTypeCombo},
		QSignalBlocker{ui.docTypeCombo}
	};

	ui.dateEdit->setDate(QDate{ inv.date.year, inv.date.month, inv.date.day });

	auto& d = inv.aggragated_amounts.taxEventDate;
	ui.taxEventDateEdit->setDate(QDate(d.year, d.month, d.day));

	ui.paymentTypeCombo->setCurrentIndex(static_cast<int>(inv.aggragated_amounts.paymentType));

	bool nzokForm = inv.nzokData.has_value();

	ui.docTypeCombo->setCurrentIndex(static_cast<int>(inv.type));

	setMainDocument(inv.mainDocument());

	ui.paymentTypeCombo->setDisabled(nzokForm);
	ui.taxEventDateEdit->setDisabled(nzokForm);
	ui.docTypeCombo->setDisabled(nzokForm);
	ui.mainDocDateEdit->setDisabled(nzokForm);
	ui.mainDocNumSpin->setDisabled(nzokForm);
	ui.addButton->setHidden(nzokForm);
	ui.deleteButton->setHidden(nzokForm);
	ui.editButton->setHidden(nzokForm);
	ui.saveXMLButton->setHidden(!nzokForm);
	ui.sendPisButton->setHidden(!nzokForm);

	//centering the label:

	QPushButton* layoutButtons[4]{ ui.addButton, ui.deleteButton, ui.editButton, ui.saveXMLButton };

	int buttonsSumWidth = 0;

	for (auto button : layoutButtons) {
		if (!button->isHidden())
			buttonsSumWidth += button->width();
	}

	ui.opLabelSpacer->changeSize(buttonsSumWidth, 0);
	

	setBusinessOperations(inv.businessOperations, inv.aggragated_amounts);

}

void FinancialView::setBusinessOperations(const BusinessOperations& businessOp, const AggregatedAmounts& amounts)
{
	m_model.setBusinessOperations(businessOp);

	ui.priceLabel->setText(formatDoubleWithDecimal(amounts.payment_amount) + " лв.");
	ui.vatLabel->setText("0%");
	ui.sumLabel->setText(formatDoubleWithDecimal(amounts.total_amount) + " лв.");
	update();
}

void FinancialView::setMainDocument(const std::optional<MainDocument>& mainDoc)
{
	if (mainDoc) {

		QSignalBlocker blockers[2]{
			QSignalBlocker(ui.mainDocDateEdit), 
			QSignalBlocker(ui.mainDocNumSpin) 
		};

		ui.mainDocNumSpin->setValue(mainDoc->number);
		ui.mainDocDateEdit->setDate(QDate(mainDoc->date.year, mainDoc->date.month, mainDoc->date.day));
		showMainDocumentDetails(true);
	}
	else {
		showMainDocumentDetails(false);
	}

}

void FinancialView::setNumberSpinBox(long long num)
{
	QSignalBlocker b(ui.numberSpinBox);
	ui.numberSpinBox->setValue(num);
}


void FinancialView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.fillRect(rect(), Theme::background);
}

void FinancialView::showMainDocumentDetails(bool show)
{
	QWidget* const mainDocWidgets[4]{

		ui.mainDocNumLabel, 
		ui.mainDocNumSpin, 
		ui.mainDocDateLabel, 
		ui.mainDocDateEdit 
	};

	for (auto& w : mainDocWidgets)
		w->setHidden(!show);
}
