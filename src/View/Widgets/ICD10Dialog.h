#pragma once

#include <QDialog>
#include "ui_ICD10Dialog.h"
#include <QSortFilterProxyModel>
#include "View/TableModels/MKBModel.h"
#include "Model/ICD10.h"

class ICD10Dialog : public QDialog
{
	Q_OBJECT

	int m_selectedRow{ -1 };

	static inline bool s_show_full_icd = false;

	static inline QSortFilterProxyModel m_proxyModel;
	MKBModel s_dental_model{ ICD10::getDentalICDCodes() };
	MKBModel s_full_model{ ICD10::getFullMKBList() };

	void initTable();

public:
	ICD10Dialog(const ICD10& code, QWidget* parent = nullptr);
	ICD10 getResult();
	~ICD10Dialog();

private:
	Ui::ICD10DialogClass ui;
};
