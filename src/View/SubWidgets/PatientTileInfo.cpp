#include "PatientTileInfo.h"

#include <QMenu>

#include "Presenter/PatientInfoPresenter.h"
#include "View/Theme.h"
#include "Model/User.h"

PatientTileInfo::PatientTileInfo(QWidget *parent)
	: RoundedFrame(parent)
{
	ui.setupUi(this);

    addVerticalSeparator(ui.patientTile->width());
    setFrameColor(Theme::border);

    //init context menu
    context_menu = new QMenu(this);

    QAction* action;

    action = (new QAction("Запази посещение", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->appointmentClicked(); });
    action->setIcon(QIcon(":/icons/icon_calendar.png"));
    context_menu->addAction(action);

    action = (new QAction("Създрай напомняне", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->notificationClicked(); });
    action->setIcon(QIcon(":/icons/icon_bell.png"));
    context_menu->addAction(action);

    action = (new QAction("Изпрати SMS", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->sendSms(); });
    action->setIcon(QIcon(":/icons/icon_sms.png"));
    context_menu->addAction(action);

	action = (new QAction("Проверка за хоспитализация", context_menu));
	connect(action, &QAction::triggered, this, [=, this] { presenter->checkActiveHospitalization(); });
	action->setIcon(QIcon(":/icons/icon_hospital.png"));
    context_menu->addAction(action);
	
	QMenu* newDocMenu = context_menu->addMenu("Отвори");
	newDocMenu->setIcon(QIcon(":/icons/icon_open.png"));

    action = (new QAction("Нов амбулаторен лист", newDocMenu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::AmbList); });
    action->setIcon(QIcon(":/icons/icon_sheet.png"));
    newDocMenu->addAction(action);

    action = (new QAction("Ново пародонтално измерване", newDocMenu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::PerioStatus); });
    action->setIcon(QIcon(":/icons/icon_periosheet.png"));
    newDocMenu->addAction(action);

    action = (new QAction("Нова рецепта", newDocMenu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::Prescription); });
    action->setIcon(QIcon(":/icons/icon_prescr.png"));
    newDocMenu->addAction(action);

    action = (new QAction("Нов лечебен план", newDocMenu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::TreatmentPlan); });
    action->setIcon(QIcon(":/icons/icon_plan.png"));
    newDocMenu->addAction(action);

    action = (new QAction("Нова фактура", newDocMenu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::Financial); });
    action->setIcon(QIcon(":/icons/icon_invoice.png"));
    newDocMenu->addAction(action);

    action = (new QAction("Пациентско досие", newDocMenu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::PatientSummary); });
    action->setIcon(QIcon(":/icons/icon_history.png"));
    newDocMenu->addAction(action);
    /*
    action = (new QAction("Редактирай", context_menu));
    connect(action, &QAction::triggered, this, [=, this] {
        context_menu_overPatient ?
            ui.patientTile->click()
            :
            ui.medStatTile->click();
        });

    action->setIcon(QIcon(":/icons/icon_edit.png"));
    context_menu->addAction(action);
    */
    context_menu->setStyleSheet(Theme::getPopupMenuStylesheet());

    //connect signalsh

    connect(ui.patientTile->nraButton, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->nraClicked(true);
	});

    connect(ui.patientTile, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->patientTileClicked();
	});

    connect(ui.medStatTile, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->medStatTileClicked();
	});

	connect (ui.patientTile->notesButton, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->notesRequested();
	});

    connect (ui.patientTile->menuButton, &QPushButton::clicked, this, [=, this]{
        context_menu_overPatient = true;

        const QPoint globalPos = ui.patientTile->menuButton->mapToGlobal(QPoint(0, ui.patientTile->menuButton->height()));

        context_menu->popup(globalPos);
    });

    connect(ui.patientTile, &TileButton::customContextMenuRequested, this, [&](QPoint point){
        context_menu_overPatient = true;
         context_menu->popup(point);
    });

    connect(ui.medStatTile, &TileButton::customContextMenuRequested, this, [&](QPoint point){
        context_menu_overPatient = false;
        context_menu->popup(point);
    });
}

void PatientTileInfo::setPatient(const Patient& p, int age)
{
	ui.patientTile->setData(p, age);
	ui.medStatTile->setData(p);
}

PatientTileInfo::~PatientTileInfo()
{}
