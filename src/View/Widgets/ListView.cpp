#include "ListView.h"

#include "View/SubWidgets/ContextMenu.h"
#include "View/Theme.h"
#include "Model/Dental/NhifSheetData.h"
#include "View/uiComponents/MouseWheelGuard.h"
#include "View/SubWidgets/ReferralTile.h"
#include "QtVersion.h"

ListView::ListView(QWidget* parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);
	
	teethViewScene = new TeethViewScene(ui.teethView);
	contextMenu = new ContextMenu();
	teethViewScene->setContextMenu(contextMenu);

	ui.ambNumSpin->setTotalLength(6);
	ui.ambNumSpin->installEventFilter(new MouseWheelGuard(ui.ambNumSpin));
	ui.dateTimeEdit->installEventFilter(new MouseWheelGuard(ui.dateTimeEdit));
	ui.specCombo->installEventFilter(new MouseWheelGuard(ui.specCombo));

	ui.teethView->setScene(teethViewScene);
	ui.teethView->setSceneRect(teethViewScene->sceneRect());
	ui.teethView->installEventFilter(this);
	ui.procedureTable->setModel(&model);
	ui.procedureTable->setAmbListLayout();

	ui.perioButton->setIcon(QIcon(":/icons/icon_periosheet.png"));
	ui.invoiceButton->setIcon(QIcon(":/icons/icon_invoice.png"));
	ui.prescrButton->setIcon(QIcon(":/icons/icon_prescr.png"));
	ui.addProcedure->setIcon(QIcon(":/icons/icon_add.png"));
	ui.deleteProcedure->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.editProcedure->setIcon(QIcon(":/icons/icon_edit.png"));
	ui.historyButton->setIcon(QIcon(":/icons/icon_history.png"));
	ui.nssiButton->setIcon(QIcon(":/icons/icon_nssi.png"));

	ui.perioButton->setHoverColor(Theme::mainBackgroundColor);
	ui.invoiceButton->setHoverColor(Theme::mainBackgroundColor);
	ui.prescrButton->setHoverColor(Theme::mainBackgroundColor);
	ui.addProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.deleteProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.editProcedure->setHoverColor(Theme::mainBackgroundColor);
	ui.historyButton->setHoverColor(Theme::mainBackgroundColor);

	QMenu* menu = new QMenu(ui.addRefButton);

	menu->setStyleSheet(Theme::getPopupMenuStylesheet());

	for (int i = 0; i < Referral::refDescription.size(); i++) {
		
		QAction* action = new QAction(Referral::refDescription[i], menu);
		menu->addAction(action);

		connect(action, &QAction::triggered, [=] { 
			presenter->addReferral(static_cast<ReferralType>(i));
			}
		);
	}

	ui.addRefButton->setMenu(menu);

	ui.addRefButton->setIcon(QIcon(":/icons/icon_add.png"));
	ui.addRefButton->setHoverColor(Theme::mainBackgroundColor);

	setStyleSheet(Theme::getFancyStylesheet());


	ui.procedureLabel->setStyleSheet(
		"color : " + Theme::colorToString(Theme::fontTurquoise) + "; "
		"font-weight: bold; font-size: 12px;"
	);

	connect(ui.nrnButton, &QPushButton::clicked, [=] { if (presenter) presenter->hisButtonPressed();});
	connect(ui.ambNumSpin, &LeadingZeroSpinBox::valueChanged, [=](long long value) {if(presenter)presenter->ambNumChanged(value);});
	connect(ui.dateTimeEdit, &QDateTimeEdit::dateTimeChanged, [=](const QDateTime& t) {if (presenter)presenter->setAmbDateTime(t.toString(Qt::ISODate).toStdString());});
	connect(ui.historyButton, &QPushButton::clicked, [=] { if (presenter) presenter->historyRequested(); });
	connect(ui.addProcedure, &QAbstractButton::clicked, [=] { if (presenter) presenter->addProcedure(); });
	connect(ui.specCombo, QtComboIndexChanged, [=] {nhifChanged();});
	connect(ui.unfavCheck, &QCheckBox::stateChanged, [=] { nhifChanged(); });
	connect(ui.nssiButton, &QPushButton::clicked, [=] { if (presenter) presenter->checkPention(); });
	connect(ui.editProcedure, &QPushButton::clicked, [=] { if (presenter) presenter->editProcedure(ui.procedureTable->selectedRow()); });
	connect(ui.invoiceButton, &QPushButton::clicked, [=] { if (presenter) presenter->createInvoice(); });
	connect(ui.perioButton, &QPushButton::clicked, [=] { if (presenter) presenter->createPerioMeasurment(); });
	connect(ui.prescrButton, &QPushButton::clicked, [=] { if (presenter) presenter->createPrescription(); });
	connect(ui.deleteProcedure, &QAbstractButton::clicked, 
		[=] {

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

	connect(ui.procedureTable, &QTableView::doubleClicked, [=] { ui.editProcedure->click(); });
	connect(ui.procedureTable, &TableView::deletePressed, [=] { if (presenter) ui.deleteProcedure->click(); });
	connect(ui.procedureTable, &TableView::rowDragged, [=] { if(presenter) presenter->moveProcedure(ui.procedureTable->selectedRow(), model.lastDroppedRowIndex()); });


	ui.controlPanel->hide();
	ui.surfacePanel->hide();

}

void ListView::setPresenter(ListPresenter* presenter)
{
	this->presenter = presenter;
	teethViewScene->setPresenter(presenter);
	ui.controlPanel->setPresenter(presenter);
	contextMenu->setPresenter(presenter);
}

void ListView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.fillRect(rect(), Theme::background);

	QPainterPath path;

	path.addRoundedRect(
		QRectF(
			ui.frame->x() + ui.teethView->x(),
			ui.frame->y() + ui.teethView->y(),
			ui.teethView->width() + ui.controlPanel->width(),
			ui.teethView->height()
		),
		Theme::radius/2,
		Theme::radius/2
	);

	painter.fillPath(path, Theme::sectionBackground);
	
	painter.setPen(QPen(Theme::border));
	painter.drawPath(path);

	painter.drawLine(
		ui.frame->x() + ui.surfacePanel->x(),
		ui.frame->y() + ui.surfacePanel->y(),
		ui.frame->x() + ui.surfacePanel->x(),
		ui.frame->y() + ui.surfacePanel->y() + ui.teethView->height()
	);

	if (!m_teethViewFocused) return;

	painter.setPen(QPen(Theme::mainBackgroundColor));
	/*
	auto teethViewPath = Theme::getHalfCurvedPath(
		ui.teethView->width(), 
		ui.teethView->height()
	);

	painter.translate(ui.teethView->pos());
	*/
	painter.drawPath(path);



}

bool ListView::eventFilter(QObject* obj, QEvent* event)
{
	if (obj != ui.teethView) return false;

	if (event->type() == QEvent::FocusOut)
	{
			m_teethViewFocused = false;
			repaint();

	}
	else if (event->type() == QEvent::FocusIn)
	{
		m_teethViewFocused = true;
		repaint();
	}


	return false;
}

void ListView::nhifChanged()
{
	if (presenter == nullptr) return;

	NhifSheetData data;

	//data.charge = static_cast<NhifCharge>(ui.taxCombo->currentIndex());
	data.specification = static_cast<NhifSpecification>(ui.specCombo->currentIndex());
	data.isUnfavourable = ui.unfavCheck->isChecked();

	presenter->setNhifData(data);
}


void ListView::setAmbListNum(int number)
{
	QSignalBlocker b(ui.ambNumSpin);
	ui.ambNumSpin->setValue(number);
}

void ListView::setDateTime(const std::string& time8601)
{
	QSignalBlocker b(ui.dateTimeEdit);
	ui.dateTimeEdit->setDateTime(QDateTime::fromString(time8601.c_str(), Qt::ISODate));
}

#include <qdebug.h>
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

ISurfacePanel* ListView::surfacePanel()
{
	return ui.surfacePanel;
}

IPatientTileInfo* ListView::tileInfo()
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

void ListView::setSelectedTeeth(std::vector<int> selectedIndexes)
{
	teethViewScene->setSelectedTeeth(selectedIndexes);

	ui.teethView->update(); //the only way to update qgraphicsview without most of the bugs

}


void ListView::setProcedures(const std::vector<Procedure>& m)
{
	model.setProcedures(m);

	std::vector<int> proc_teeth;
	proc_teeth.reserve(m.size());

	for (auto t : m) proc_teeth.push_back(t.tooth_idx.index);

	teethViewScene->setProcedures(proc_teeth);
}

void ListView::hideNhifSheetData()
{
	ui.spec_label->hide();
	ui.specCombo->hide();
	ui.unfavCheck->hide();
	ui.nssiButton->hide();

}

void ListView::setNhifData(const NhifSheetData& data, bool showUnfav)
{
	ui.spec_label->show();
	ui.specCombo->show();
	ui.nssiButton->show();

	QSignalBlocker b(ui.specCombo);
	ui.specCombo->setCurrentIndex(static_cast<int>(data.specification));

	QSignalBlocker b1(ui.unfavCheck);
	ui.unfavCheck->setChecked(data.isUnfavourable);

	ui.unfavCheck->setHidden(!showUnfav);

}

void ListView::setReferrals(const std::vector<Referral>& referrals)
{
	while (ui.referralLayout->count())
	{
		ui.referralLayout->takeAt(0)->widget()->deleteLater();
	}


	for (int i = 0; i < referrals.size(); i++) {
		ReferralTile* refWidget = new ReferralTile(referrals[i], i, this);
		connect(refWidget, &ReferralTile::remove, [=](int index) {presenter->removeReferral(index);});
		connect(refWidget, &ReferralTile::clicked, [=](int index) {presenter->editReferral(index);});
		connect(refWidget, &ReferralTile::print, [=](int index) {presenter->printReferral(index);});
		connect(refWidget, &ReferralTile::sendToHis, [=](int index) {presenter->sendReferralToHis(index);});
		ui.referralLayout->addWidget(refWidget);
		
	}

}




void ListView::setHisButtonText(HisButtonProperties prop)
{
	ui.ambNumSpin->setHidden(prop.hideSpinBox);
	ui.label->setText(prop.labelText.c_str());
	ui.nrnButton->setText(prop.buttonText.c_str());
	ui.nrnButton->setHoverText(prop.hoverText.c_str());
}


ListView::~ListView()
{
	delete teethViewScene;
	delete contextMenu;
}
