#pragma once

#include <QWidget>
#include "ui_ProcedureInput.h"
#include "Model/Validators/CommonValidators.h"
#include "View/Interfaces/IProcedureInput.h"

class DateEdit;


class ProcedureInput : public QWidget, public IProcedureInput
{
	Q_OBJECT

	void initView(const ProcedureCode& code);

	ProcedureCode m_code;

	bool m_postDisabled = false;

	//financing combo logic
	void initFinancingCombo(const ProcedureCode& code);
	void setFinancingSource(FinancingSource source);
	FinancingSource getFinancingSource();

public:
	ProcedureInput(QWidget *parent = Q_NULLPTR);

	~ProcedureInput();

	QDateEdit* qDateEdit();

	virtual AbstractDateEdit* dateEdit();

	Ui::ProcedureInput ui;


	// Inherited via IProcedureInput
	void setData(const Data& data) override;

	void setErrorMsg(const std::string& errorMsg);
	void disablePost() override;

	Data getData() override;

	std::string isValid() override;

};
