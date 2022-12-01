#pragma once

#include <QDialog>
#include <QSortFilterProxyModel>
#include "ui_KSMPDialog.h"
#include "Model/Dental/KSMP.h"
#include "Model/Dental/MKB.h"

class KSMPDialog : public QDialog
{
	Q_OBJECT

	int m_selectedRow{ -1 };

	void initCommon(); //common init fn for both of the constructors

	QSortFilterProxyModel m_proxyModel;
	QAbstractTableModel* m_model;

public:
	KSMPDialog(KsmpList& list, const std::string& code = std::string(), QWidget* parent = nullptr);
	KSMPDialog(const std::vector<MKB>& mkbList, const MKB& mkb_default = MKB{});
	std::string getResult();
	~KSMPDialog();

private:
	Ui::KSMPDialogClass ui;
};
