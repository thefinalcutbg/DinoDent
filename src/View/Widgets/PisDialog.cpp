﻿#include "PisDialog.h"

PisDialog::PisDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle(u8"Месечни отчети и известия");
	setWindowIcon(QIcon(":/icons/icon_nzok.png"));

	ui.monthNotifView->setParentDialog(this);

	ui.tabWidget->setStyleSheet(
		"QTabBar::tab:selected {background-color: white}"
	);
}

PisDialog::~PisDialog()
{}