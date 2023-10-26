#include "PisDialog.h"

PisDialog::PisDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Месечни отчети и известия");
	setWindowIcon(QIcon(":/icons/icon_nhif.png"));

	ui.monthNotifView->setParentDialog(this);

	ui.tabWidget->tabBar()->setTabIcon(0, QIcon(":/icons/icon_sheet.png"));
	ui.tabWidget->tabBar()->setTabIcon(1, QIcon(":/icons/icon_invoice.png"));
	ui.tabWidget->tabBar()->setTabIcon(2, QIcon(":/icons/icon_apply.png"));
	ui.tabWidget->tabBar()->setTabIcon(3, QIcon(":/icons/icon_nssi.png"));
	ui.tabWidget->tabBar()->setTabIcon(4, QIcon(":/icons/icon_db.png"));

	ui.tabWidget->setStyleSheet(
		"QTabBar::tab:selected {background-color: white}"
	);
}

PisDialog::~PisDialog()
{}
