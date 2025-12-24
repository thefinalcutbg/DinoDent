#include "PatientTileInfo.h"

#include <QMenu>

#include "Presenter/PatientInfoPresenter.h"
#include "View/Theme.h"

PatientTileInfo::PatientTileInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    //init context menu
    context_menu = new QMenu(this);

    QAction* action;

    action = (new QAction("Редактирай", context_menu));
    connect(action, &QAction::triggered, this, [=, this] {
        context_menu_overPatient ?
        ui.patientTile->click()
        :
        ui.medStatTile->click();
    });
    action->setIcon(QIcon(":/icons/icon_edit.png"));
    context_menu->addAction(action);

    action = (new QAction("Нов амбулаторен лист", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::AmbList); });
    action->setIcon(QIcon(":/icons/icon_sheet.png"));
    context_menu->addAction(action);

    action = (new QAction("Ново пародонтално измерване", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::PerioStatus); });
    action->setIcon(QIcon(":/icons/icon_periosheet.png"));
    context_menu->addAction(action);

    action = (new QAction("Нова рецепта", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::Prescription); });
    action->setIcon(QIcon(":/icons/icon_prescr.png"));
    context_menu->addAction(action);

    action = (new QAction("Нов план на лечение", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::TreatmentPlan); });
    action->setIcon(QIcon(":/icons/icon_plan.png"));
    context_menu->addAction(action);

    action = (new QAction("Нова фактура", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::Financial); });
    action->setIcon(QIcon(":/icons/icon_invoice.png"));
    context_menu->addAction(action);

    action = (new QAction("Запази посещение", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::Calendar); });
    action->setIcon(QIcon(":/icons/icon_calendar.png"));
    context_menu->addAction(action);

    action = (new QAction("Пациентско досие", context_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter->openDocument(TabType::PatientSummary); });
    action->setIcon(QIcon(":/icons/icon_history.png"));
    context_menu->addAction(action);

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

	connect (ui.patientTile->appointmentButton, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->appointmentClicked();
	});

    connect (ui.patientTile->notificationButton, &QPushButton::clicked, this, [=, this]{
        if (presenter) presenter->notificationClicked();
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
