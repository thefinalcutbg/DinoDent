#include "PisDialog.h"
#include "Model/User.h"

PisDialog::PisDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Месечни отчети и известия");
	setWindowIcon(QIcon(":/icons/icon_nhif.png"));
	setWindowFlag(Qt::WindowMaximizeButtonHint);

	ui.monthNotifView->setParentDialog(this);

	ui.tabWidget->tabBar()->setTabIcon(0, QIcon(":/icons/icon_sheet.png"));
	ui.tabWidget->tabBar()->setTabIcon(1, QIcon(":/icons/icon_invoice.png"));
	ui.tabWidget->tabBar()->setTabIcon(2, QIcon(":/icons/icon_apply.png"));
	ui.tabWidget->tabBar()->setTabIcon(3, QIcon(":/icons/icon_nssi.png"));
	ui.tabWidget->tabBar()->setTabIcon(4, QIcon(":/icons/icon_db.png"));
	ui.tabWidget->tabBar()->setTabIcon(5, QIcon(":/icons/icon_nhif.png"));

	if (!User::hasNhifContract() ||
		User::practice().nhif_contract->nssi_pass.empty()) {
		ui.tabWidget->removeTab(3);
	}

	connect(ui.tabWidget, &QTabWidget::currentChanged, this, [=](int index) {

		if (index == 1) {
			ui.monthNotifView->hasBeenShown();
		}
		else if (index == 2) {
			ui.filesView->hasBeenShown();
		}
	});
}

PisDialog::~PisDialog()
{}
