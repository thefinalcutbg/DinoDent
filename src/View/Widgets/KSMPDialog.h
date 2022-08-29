#pragma once

#include <QDialog>
#include <QSortFilterProxyModel>
#include "ui_KSMPDialog.h"
#include "View/Models/KSMPModel.h"

class KSMPDialog : public QDialog
{
	Q_OBJECT

	int m_selectedRow{ -1 };

	QSortFilterProxyModel m_proxyModel;
	KSMPModel m_model;

public:
	KSMPDialog(KsmpList& list, const std::string& code = std::string(), QWidget* parent = nullptr);
	std::string getResult();
	~KSMPDialog();

private:
	Ui::KSMPDialogClass ui;
};
