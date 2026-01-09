#include "ListView.h"

#include "View/SubWidgets/ContextMenu.h"
#include "View/Theme.h"
#include "Model/Dental/NhifSheetData.h"
#include "View/uiComponents/MouseWheelGuard.h"
#include "View/SubWidgets/ReferralTile.h"
#include "View/SubWidgets/MedicalNoticeTile.h"
#include "Model/User.h"
#include <QIcon>

ListView::ListView(QWidget* parent)
	: ShadowBakeWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);

	setShadowTargets({
		ui.patientInfoTile->getFrame(),
		ui.frame,
		ui.procedureFrame
		}
	);

	teethViewScene = new TeethViewScene(ui.teethView);
	contextMenu = new ContextMenu();
	teethViewScene->setContextMenu(contextMenu);
    ui.sigButton->setGraphicsEffect(nullptr);
    ui.frame->addVerticalSeparator(ui.teethView->width());

    ui.procedureFrame->setDynamicFocusBorderChange();
    ui.frame->setDynamicFocusBorderChange();

    ui.nrnButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);

	ui.dateTimeEdit->installEventFilter(new MouseWheelGuard(ui.dateTimeEdit));
	ui.specCombo->installEventFilter(new MouseWheelGuard(ui.specCombo));

	ui.teethView->setScene(teethViewScene);
	ui.teethView->setSceneRect(teethViewScene->sceneRect());
	ui.teethView->installEventFilter(this);

	ui.procedureTable->setModel(&model);
	ui.procedureTable->setAmbListLayout();

	ui.addProcedure->setIcon(QIcon(":/icons/icon_add.png"));
    ui.plannedProcedure->setIcon(QIcon(":/icons/icon_addPlanned.png"));
	ui.statusResultButton->setIcon(QIcon(":/icons/icon_apply.png"));
	ui.deleteProcedure->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.editProcedure->setIcon(QIcon(":/icons/icon_edit.png"));
	ui.historyButton->setIcon(QIcon(":/icons/icon_history.png"));
	ui.nssiButton->setIcon(QIcon(":/icons/icon_nssi.png"));
    ui.syncButton->setIcon(QIcon(":/icons/icon_sync.png"));

    ui.perioButton->setIcon(QIcon(":/icons/icon_add.png"));
    ui.invoiceButton->setIcon(QIcon(":/icons/icon_add.png"));
    ui.prescrButton->setIcon(QIcon(":/icons/icon_add.png"));
	ui.medicalNoticeButton->setIcon(QIcon(":/icons/icon_add.png"));
	ui.addRefButton->setIcon(QIcon(":/icons/icon_add.png"));
    ui.treatmentPlanButton->setIcon(QIcon(":/icons/icon_add.png"));
    ui.declarationButton->setIcon(QIcon(":/icons/icon_print.png"));

	ui.perioButton->setHoverColor(Theme::mainBackgroundColor);
	ui.invoiceButton->setHoverColor(Theme::mainBackgroundColor);
	ui.prescrButton->setHoverColor(Theme::mainBackgroundColor);
	ui.addProcedure->setHoverColor(Theme::mainBackgroundColor);
    ui.plannedProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.deleteProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.editProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.historyButton->setHoverColor(Theme::mainBackgroundColor);
	ui.nssiButton->setHoverColor(Theme::mainBackgroundColor);
	ui.statusResultButton->setHoverColor(Theme::mainBackgroundColor);
	ui.addRefButton->setHoverColor(Theme::mainBackgroundColor);
    ui.declarationButton->setHoverColor(Theme::mainBackgroundColor);
	ui.medicalNoticeButton->setHoverColor(Theme::mainBackgroundColor);
    ui.treatmentPlanButton->setHoverColor(Theme::mainBackgroundColor);
    ui.syncButton->setHoverColor(Theme::mainBackgroundColor);

    QMenu* menu = new QMenu(ui.addRefButton);

	menu->setStyleSheet(Theme::getPopupMenuStylesheet());

    for (std::size_t i = 0; i < Referral::refDescription.size(); i++) {
		
		QAction* action = new QAction(Referral::refDescription[i], menu);
		menu->addAction(action);

        connect(action, &QAction::triggered, this, [=, this] {
			presenter->addReferral(static_cast<ReferralType>(i));
			}
		);
	}

	ui.addRefButton->setMenu(menu);
	
    setStyleSheet(Theme::getFancyStylesheet());

	ui.procedureLabel->setStyleSheet(
		"color : " + Theme::colorToString(Theme::fontTurquoise) + "; "
		"font-weight: bold; font-size: 12px;"
	);

	ui.otherDocsLabel->setStyleSheet(
		"color : " + Theme::colorToString(Theme::fontTurquoise) + "; "
		"font-weight: bold; font-size: 12px;"
	);

	ui.procedureTable->setMinimumWidth(ui.teethView->width() + ui.controlPanel->width());

    connect(ui.syncButton, &QPushButton::clicked, this, [=, this] { presenter->syncWithHis();});
    connect(ui.sigButton, &QPushButton::clicked, this, [=, this] { if(presenter) presenter->showSignature(); });
    connect(ui.pentionTaxButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->addFinancialReceipt(); });
    connect(ui.nrnButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->hisButtonPressed();});
    connect(ui.dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, [=, this] (const QDateTime& t) {if (presenter)presenter->setAmbDateTime(t.toString(Qt::ISODate).toStdString());});
	connect(ui.treatmentEnd, &QDateTimeEdit::dateTimeChanged, this, [=, this](const QDateTime& t) {if (presenter)presenter->setTreatmentEndTime(t.toString(Qt::ISODate).toStdString()); });
	connect(ui.historyButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->historyRequested(); });
	connect(ui.addProcedure, &QAbstractButton::clicked, this, [=, this] { if (presenter) presenter->addProcedure(); });
    connect(ui.plannedProcedure, &QAbstractButton::clicked, this, [=, this] { if (presenter) presenter->addPlannedProcedure(); });
    connect(ui.specCombo, &QComboBox::currentIndexChanged, this, [=, this]  {nhifChanged();});
    connect(ui.unfavCheck, &QCheckBox::checkStateChanged, this, [=, this] { nhifChanged(); });
    connect(ui.nssiButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->checkPention(); });
    connect(ui.editProcedure, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->editProcedure(ui.procedureTable->selectedRow()); });
	connect(ui.statusResultButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->showAppliedStatus(); });
    connect(ui.invoiceButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->createInvoice(); });
    connect(ui.perioButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->createPerioMeasurment(); });
    connect(ui.prescrButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->createPrescription(); });
    connect(ui.medicalNoticeButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->addMedicalNotice(); });
    connect(ui.declarationButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->printDeclarations(); });
    connect(ui.treatmentPlanButton, &QPushButton::clicked, this, [=, this] { if (presenter) presenter->createTreatmentPlan(); });
    connect(ui.deleteProcedure, &QAbstractButton::clicked, this, [=, this]
        {

			if (!presenter) return;

			int currentIdx = ui.procedureTable->selectedRow();
			int lastIdx = ui.procedureTable->verticalHeader()->count()-1;

//			if (currentIdx == -1) return;

			presenter->deleteProcedure(currentIdx);

			if (currentIdx == lastIdx)
			{
				ui.procedureTable->selectRow(currentIdx - 1);
			}
			else ui.procedureTable->selectRow(currentIdx);
		});

    connect(ui.procedureTable, &TableView::deletePressed, this, [=, this]() { ui.deleteProcedure->clicked(); });
    connect(ui.procedureTable, &TableView::editPressed, this, [=, this](int row) { if (presenter) presenter->editProcedure(row); });
    connect(ui.procedureTable, &TableView::rowDragged, this, [=, this] {

		int from = ui.procedureTable->selectedRow();
		int to = model.lastDroppedRowIndex();

		if (from == to) to--;

		if(presenter) presenter->moveProcedure(from, to);
	}
	);

	ui.controlPanel->hide();
	ui.surfacePanel->hide();
	ui.procedureTable->enableContextMenu(true);

	ui.nrnButton->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.nrnButton, &QPushButton::customContextMenuRequested, [=, this](const QPoint& pos) {

        QMenu* hisMenu = new QMenu(this);

		auto text = ui.nrnButton->text();

		//if signature tablet is configured
		if (User::signatureTablet().getHisIdx()) {

			QAction* action = new QAction("Изпрати с подпис на пациента", hisMenu);

			action->setIcon(QIcon(":/icons/icon_sign.png"));

			connect(action, &QAction::triggered, this, [=, this] {
				presenter->sendToHis(true);
			});

			hisMenu->addAction(action);

			action = new QAction("Изпрати с подпис на родител/настойник", hisMenu);

			action->setIcon(QIcon(":/icons/icon_sign.png"));

			connect(action, &QAction::triggered, this, [=, this] {
				presenter->sendToHis(false);
				});

			hisMenu->addAction(action);
		}

        if(text != "Изпрати към НЗИС"){
            QAction* action = new QAction("Синхронизирай с НЗИС", hisMenu);

            action->setIcon(QIcon(":/icons/icon_sync.png"));

            connect(action, &QAction::triggered, this, [=, this] {
                presenter->syncWithHis();
            });

            hisMenu->addAction(action);

            action = new QAction("Анулирай", hisMenu);

            action->setIcon(QIcon(":/icons/icon_remove.png"));

            connect(action, &QAction::triggered, this, [=, this] {
                presenter->cancelHisAmbList();
            });

            hisMenu->addAction(action);
        }
	
		hisMenu->setStyleSheet(Theme::getPopupMenuStylesheet());

		hisMenu->exec(ui.nrnButton->mapToGlobal(pos));
	});
}

void ListView::setPresenter(ListPresenter* presenter)
{
	this->presenter = presenter;
	teethViewScene->setPresenter(presenter);
	ui.controlPanel->setPresenter(presenter);
	contextMenu->setPresenter(presenter);
}

bool ListView::eventFilter(QObject* obj, QEvent* event)
{
    if (obj != ui.teethView) return false;

    if (event->type() == QEvent::FocusOut)
    {
        ui.surfacePanel->drawFocused(false);
        teethViewScene->drawFocused(false);

        repaint();
    }
    else if (event->type() == QEvent::FocusIn)
    {
        ui.surfacePanel->drawFocused(true);
        teethViewScene->drawFocused(true);

        repaint();
    }

    return false;
}

void ListView::captureViewport()
{
	Theme::applyShadow(ui.frame);
	Theme::applyShadow(ui.procedureFrame);
	ui.frame->update();
	ui.procedureFrame->update();
	// Ensure correct DPI
	const qreal dpr = devicePixelRatioF();

	QPixmap pm(size() * dpr);
	pm.setDevicePixelRatio(dpr);
	pm.fill(Qt::transparent);

	// Render THIS widget (and its children) into pm
	// QWidget::render renders children too by default.
	render(&pm, QPoint(), QRegion(), QWidget::DrawWindowBackground | QWidget::DrawChildren);

	m_captured = pm;
	ui.frame->setGraphicsEffect(nullptr);
	ui.procedureFrame->setGraphicsEffect(nullptr);
	update();
}

void ListView::nhifChanged()
{
	if (presenter == nullptr) return;

	NhifSheetData data;

	//data.charge = static_cast<NhifCharge>(ui.taxCombo->currentIndex());
	data.specification = static_cast<NhifSpecificationType>(ui.specCombo->currentIndex());
	data.isUnfavourable = ui.unfavCheck->isChecked();

	presenter->setNhifData(data);
}

void ListView::focusTeethView()
{
	ui.teethView->setFocus();
}

void ListView::setDateTime(const std::string& time8601)
{
	QSignalBlocker b(ui.dateTimeEdit);
	ui.dateTimeEdit->setDateTime(QDateTime::fromString(time8601.c_str(), Qt::ISODate));
}

void ListView::setTreatmentEnd(const std::string& time8601)
{
	QSignalBlocker b(ui.treatmentEnd);
	ui.treatmentEnd->setDateTime(QDateTime::fromString(time8601.c_str(), Qt::ISODate));
}

void ListView::setSignature(const std::vector<unsigned char>& s)
{
	if (s.empty()) {
		ui.sigButton->setIcon(QIcon());
		ui.sigButton->setBackgroundColor(Theme::background);
		ui.sigButton->setHoverColor(Theme::background);
		ui.sigButton->setOutlineColor(Theme::background);
		ui.sigButton->setDisabled(true);
		ui.sigButton->setToolTip("");
		return;
	}

	QPixmap sigPx = QPixmap::fromImage(QImage::fromData(s.data(), static_cast<int>(s.size())));


	ui.sigButton->setEnabled(true);
	ui.sigButton->setIcon(QIcon(sigPx));
	ui.sigButton->setBackgroundColor(Qt::white);
	ui.sigButton->setOutlineColor(Theme::mainBackgroundColor);
	ui.sigButton->setHoverColor(Theme::background);
	ui.sigButton->setToolTip("Преглед на подпис");

}

void ListView::setCheckModel(const CheckModel& checkModel, const CheckModel& dsnCheckModel)
{
	ui.controlPanel->setModel(checkModel, dsnCheckModel);
	contextMenu->setModel(checkModel, dsnCheckModel);
}

void ListView::hideSurfacePanel(bool hidden)
{
	ui.surfacePanel->hidePanel(hidden);
	ui.controlPanel->hideCommonButtons(!hidden);
}

void ListView::hideControlPanel(bool hidden)
{
	ui.controlPanel->setHidden(hidden);
}

SurfacePanel* ListView::surfacePanel()
{
	return ui.surfacePanel;
}

PatientTileInfo* ListView::tileInfo()
{
	return ui.patientInfoTile;
}

void ListView::repaintTooth(const ToothPaintHint& tooth)
{
	teethViewScene->display(tooth);
    ui.teethView->setFocus();
}

void ListView::setNotes(const std::array<std::string, 32>& notes)
{
	teethViewScene->setNotes(notes);
}

void ListView::setSelectedTeeth(const std::vector<int>& selectedIndexes)
{
	teethViewScene->setSelectedTeeth(selectedIndexes);

	ui.teethView->update(); //the only way to update qgraphicsview without most of the bugs

}


void ListView::setProcedures(const std::vector<Procedure>& m)
{
	model.setProcedures(m);

	std::vector<int> proc_teeth;
	proc_teeth.reserve(32);

	for (auto& t : m) {
		for (auto i : t.getArrayIndexes()) {
			proc_teeth.push_back(i);
		}
	}

	teethViewScene->setProcedures(proc_teeth);
}

void ListView::hideNhifSheetData()
{
	ui.spec_label->hide();
	ui.specCombo->hide();
	ui.unfavCheck->hide();
	ui.nssiButton->hide();
	ui.pentionTaxButton->hide();
	ui.procedureTable->hideColumn(5);
}

void ListView::setNhifData(const NhifSheetData& data, bool showUnfav)
{
	ui.spec_label->show();
	ui.specCombo->show();
	ui.nssiButton->show();
	ui.pentionTaxButton->show();
	ui.procedureTable->showColumn(5);

	QSignalBlocker b(ui.specCombo);
	ui.specCombo->setCurrentIndex(static_cast<int>(data.specification));

	QSignalBlocker b1(ui.unfavCheck);
	ui.unfavCheck->setChecked(data.isUnfavourable);

	ui.unfavCheck->setHidden(!showUnfav);

}

void ListView::setAdditionalDocuments(const std::vector<Referral>& referrals, const std::vector<MedicalNotice>& notices)
{
	while (ui.docsLayout->count())
	{
		ui.docsLayout->takeAt(0)->widget()->deleteLater();
	}


    for (std::size_t i = 0; i < referrals.size(); i++) {
		ReferralTile* refWidget = new ReferralTile(referrals[i], i, this);
        connect(refWidget, &ReferralTile::remove, this, [=, this](int index) {presenter->removeReferral(index); });
        connect(refWidget, &ReferralTile::clicked, this, [=, this](int index) {presenter->editReferral(index); });
        connect(refWidget, &ReferralTile::print,  this, [=, this](int index) {presenter->printReferral(index); });
        connect(refWidget, &ReferralTile::sendToHis, this, [=, this](int index) {presenter->sendReferralToHis(index); });
		ui.docsLayout->addWidget(refWidget);
	}

    for (std::size_t i = 0; i < notices.size(); i++) {
		auto noticeWidget = new MedicalNoticeTile(notices[i], i, this);
        connect(noticeWidget, &MedicalNoticeTile::remove, this, [=, this](int index) {presenter->removeMedicalNotice(index); });
        connect(noticeWidget, &MedicalNoticeTile::clicked, this, [=, this](int index) {presenter->editMedicalNotice(index); });
        connect(noticeWidget, &MedicalNoticeTile::sendToHis, this, [=, this](int index) {presenter->sendMedicalNoticeToHis(index); });
		ui.docsLayout->addWidget(noticeWidget);

	}
}


void ListView::setHisButtonText(const HisButtonProperties& prop)
{
	ui.nrnButton->setText(prop.buttonText.c_str());
	ui.nrnButton->setHoverText(prop.hoverText.c_str());

    bool showSync = false;//prop.buttonText != "Изпрати към НЗИС";
    ui.syncButton->setHidden(!showSync);
}

void ListView::showAddPlannedButton(bool show)
{
    ui.plannedProcedure->setHidden(!show);
}


ListView::~ListView()
{
	delete teethViewScene;
	delete contextMenu;
}
