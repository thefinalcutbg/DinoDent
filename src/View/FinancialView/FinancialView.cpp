#include <QFileDialog>

#include "FinancialView.h"
#include "Model/Financial/Invoice.h"
#include "View/Theme.h"
#include "View/GlobalFunctions.h"

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
	ui.dateEdit->setDate(QDate{ inv.date.year, inv.date.month, inv.date.day });

	bool nzokForm = inv.nzokData.has_value();

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

	ui.opLabelSpacer->changeSize(buttonsSumWidth, 10);
	

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