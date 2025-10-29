#include <QFileDialog>

#include "FinancialView.h"
#include "Model/Financial/Invoice.h"
#include "View/Theme.h"
#include "View/GlobalFunctions.h"
#include "View/uiComponents/MouseWheelGuard.h"

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

    ui.saveXMLButton->setHoverColor(Theme::mainBackgroundColor);
    ui.addButton->setHoverColor(Theme::mainBackgroundColor);
    ui.deleteButton->setHoverColor(Theme::mainBackgroundColor);
    ui.editButton->setHoverColor(Theme::mainBackgroundColor);

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

    connect(ui.recipientButton, &QPushButton::clicked, this, [=, this] {presenter->editRecipient();});
	connect(ui.issuerButton, &QPushButton::clicked, this, [=, this] {presenter->editIssuer(); });

    connect(ui.dateEdit, &QDateEdit::dateChanged, this,
        [=, this](QDate date){
			if (presenter == nullptr) return;
			presenter->dateChanged(Date(date.day(), date.month(), date.year()));
		});

    connect(ui.taxEventDateEdit, &QDateEdit::dateChanged, this,
        [=, this](QDate date) {
			if (presenter == nullptr) return;
			presenter->taxEventDateChanged(Date(date.day(), date.month(), date.year()));
		});

    connect(ui.paymentTypeCombo, &QComboBox::currentIndexChanged, this,
        [=, this](int index) {
			presenter->paymentTypeChanged(static_cast<PaymentType>(index));
		});

    connect(ui.saveXMLButton, &QPushButton::clicked, this, [=, this] {presenter->saveAsXML(); });
    connect(ui.sendPisButton, &QPushButton::clicked, this, [=, this] {presenter->sendToPis();});

    connect(ui.numberSpinBox, &LeadingZeroSpinBox::valueChanged, this, [=, this](long long num) {if (presenter)presenter->invoiceNumberChanged(num);});

    connect(ui.deleteButton, &QPushButton::clicked, this,
        [=, this] {

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


    connect(ui.editButton, &QPushButton::clicked, this, [=, this]{ if (presenter) presenter->editOperation(ui.operationsTable->selectedRow());});

    connect(ui.addButton, &QAbstractButton::clicked, this, [=, this] { if (presenter) presenter->addOperation(); });

    connect(ui.docTypeCombo, &QComboBox::currentIndexChanged, this, [=, this](int idx) { presenter->docTypeChanged(idx);});
	
    connect(ui.mainDocDateEdit, &QDateEdit::dateChanged, this, [=, this] (QDate d) {
		presenter->mainDocumentChanged(ui.mainDocNumSpin->value(), Date(d.day(), d.month(), d.year()));
		});
    connect(ui.mainDocNumSpin, &LeadingZeroSpinBox::valueChanged, this, [=, this](long long value) {
		auto d = ui.mainDocDateEdit->date();
		presenter->mainDocumentChanged(value, Date(d.day(), d.month(), d.year()));
		});

    connect(ui.operationsTable, &TableView::deletePressed, this, [=, this](int index) { if (presenter) presenter->removeOperation(index); });
    connect(ui.operationsTable, &TableView::editPressed, this, [=, this](int index) { if (presenter) presenter->editOperation(index); });

    connect(ui.vatCheckBox, &QCheckBox::checkStateChanged, this, [=, this](Qt::CheckState state) {
        if(presenter) presenter->vatChanged(state == Qt::CheckState::Checked);
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
    auto issuer = inv.issuer();

    ui.issuerButton->setIssuer(issuer);
	ui.recipientButton->setRecipient(inv.recipient);

    QSignalBlocker blockers[5]{
		QSignalBlocker{ui.dateEdit},
		QSignalBlocker{ui.taxEventDateEdit},
		QSignalBlocker{ui.paymentTypeCombo},
        QSignalBlocker{ui.docTypeCombo},
        QSignalBlocker{ui.vatCheckBox}
	};

	ui.dateEdit->setDate(QDate{ inv.date.year, inv.date.month, inv.date.day });

	auto& d = inv.taxEventDate;
	ui.taxEventDateEdit->setDate(QDate(d.year, d.month, d.day));

	ui.paymentTypeCombo->setCurrentIndex(static_cast<int>(inv.paymentType));

	bool nzokForm = inv.nhifData.has_value();

	ui.docTypeCombo->setCurrentIndex(static_cast<int>(inv.type));

    ui.vatCheckBox->setChecked(inv.isVAT);

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
	ui.operationsTable->enableContextMenu(!nzokForm);
    ui.vatCheckBox->setDisabled(nzokForm || !issuer.vat());

	//centering the label:

	QPushButton* layoutButtons[4]{ ui.addButton, ui.deleteButton, ui.editButton, ui.saveXMLButton };

	int buttonsSumWidth = 0;

	for (auto button : layoutButtons) {
		if (!button->isHidden())
			buttonsSumWidth += button->width();
	}

	//ui.opLabelSpacer->changeSize(buttonsSumWidth, 0);


    setBusinessOperations(inv.businessOperations, inv.amount(), inv.isVAT);

}

void FinancialView::setBusinessOperations(const BusinessOperations& businessOp, double amount, bool hasVAT)
{
	m_model.setBusinessOperations(businessOp);

	ui.priceLabel->setText(formatDoubleWithDecimal(amount) + " лв.");

    ui.vatCheckBox->setText( hasVAT ? "ДДС 20%:" : "ДДС 0%:" );

    ui.vatLabel->setText(formatDoubleWithDecimal(hasVAT ? amount*0.2 : 0) + " лв.");

    if(hasVAT){
        amount = amount + (0.2*amount);
    }

	ui.sumLabel->setText(formatDoubleWithDecimal(amount) + " лв.");
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


void FinancialView::paintEvent(QPaintEvent*)
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
