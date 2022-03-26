#include <QFileDialog>

#include "FinancialView.h"
#include "Model/Financial/Invoice.h"
#include "View/Theme.h"
#include "View/GlobalFunctions.h"
#include <QDebug>

FinancialView::FinancialView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.operationsTable->setModel(&m_model);
	ui.operationsTable->setBusinessOperationLayout();
	ui.operationsTable->setStyleSheet(
		"color :" + Theme::getRGBStringFromColor(Theme::fontTurquoise) + "; "
		"selection-color:" + Theme::getRGBStringFromColor(Theme::fontTurquoiseClicked) + "; "
		"selection-background-color: " + Theme::getRGBStringFromColor(Theme::background) + "; "
	);

	setStyleSheet("QLabel{ color :" + Theme::getRGBStringFromColor(Theme::fontTurquoise) + ";}");

	ui.operationsLabel->setStyleSheet(
		"color : " + Theme::getRGBStringFromColor(Theme::fontTurquoise) + "; "
		 "font-weight: bold; font-size: 12px;"
	);

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

	connect(ui.paymentTypeCombo, &QComboBox::currentIndexChanged,
		[=](int index) {
			presenter->paymentTypeChanged(static_cast<PaymentType>(index));
		});

	connect(ui.saveXMLButton, &QPushButton::clicked, [=]
		{ presenter->saveAsXML(); });



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


	connect(ui.operationsTable, &ProcedureTable::deletePressed,[=] { ui.deleteButton->click(); });
	connect(ui.operationsTable, &QTableView::doubleClicked, [=] { ui.editButton->click(); });
	
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

	QSignalBlocker a(ui.dateEdit);
	ui.dateEdit->setDate(QDate{ inv.date.year, inv.date.month, inv.date.day });

	QSignalBlocker b(ui.taxEventDateEdit);
	auto& d = inv.aggragated_amounts.taxEventDate;
	ui.taxEventDateEdit->setDate(QDate(d.year, d.month, d.day));

	QSignalBlocker c(ui.paymentTypeCombo);
	ui.paymentTypeCombo->setCurrentIndex(static_cast<int>(inv.aggragated_amounts.paymentType));

	bool nzokForm = inv.nzokData.has_value();

	ui.paymentTypeCombo->setDisabled(nzokForm);
	ui.taxEventDateEdit->setDisabled(nzokForm);
	ui.addButton->setHidden(nzokForm);
	ui.deleteButton->setHidden(nzokForm);
	ui.editButton->setHidden(nzokForm);
	ui.saveXMLButton->setHidden(!nzokForm);

	//centering the label:

	QPushButton* layoutButtons[4]{ ui.addButton, ui.deleteButton, ui.editButton, ui.saveXMLButton };

	int buttonsSumWidth = 0;

	for (auto button : layoutButtons) {
		if (!button->isHidden())
			buttonsSumWidth += button->width();
	}

	//auto hint = ui.opLabelSpacer->sizeHint();

	//hint.setWidth(buttonsSumWidth);

	ui.opLabelSpacer->changeSize(buttonsSumWidth, 0);
	

	setBusinessOperations(inv.businessOperations, inv.aggragated_amounts);

}

void FinancialView::setBusinessOperations(const BusinessOperations& businessOp, const AggregatedAmounts& amounts)
{
	m_model.setBusinessOperations(businessOp);

	ui.priceLabel->setText(formatDoubleWithDecimal(amounts.payment_amount) + u8" лв.");
	ui.vatLabel->setText(u8"0%");
	ui.sumLabel->setText(formatDoubleWithDecimal(amounts.total_amount) + u8" лв.");
}

void FinancialView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.fillRect(rect(), Theme::background);
}