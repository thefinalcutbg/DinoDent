#pragma once

#include <QDialog>
#include "ui_DbSettingsDialog.h"

#include "Model/GlobalSettingsData.h"

QT_BEGIN_NAMESPACE
namespace Ui { class DbSettingsDialogClass; };
QT_END_NAMESPACE



class DbSettingsDialog : protected QDialog
{
	Q_OBJECT

public:
	DbSettingsDialog(const DbSettings& s, QWidget *parent = nullptr);

	std::optional<DbSettings> getResult();

	~DbSettingsDialog();

private:
	Ui::DbSettingsDialogClass *ui;
};

