#pragma once

#include <QWidget>
#include "ui_ProcedureInput.h"
#include "Model/Validators/CommonValidators.h"
#include "View/Interfaces/IProcedureInput.h"
#include "View/TableModels/MKBModel.h"

class DateEdit;


class ProcedureInput : public QWidget, public IProcedureInput
{
	Q_OBJECT


	bool m_postDisabled = false;

	bool m_allow_singleRange = false;

	//financing combo logic
	void initFinancingCombo(bool hasNhifCode);
	void setFinancingSource(FinancingSource source);
	FinancingSource getFinancingSource();

	ConstructionRange getConstructionRange();
	RestorationData getRestorationData();


public:
	ProcedureInput(QWidget *parent = Q_NULLPTR);

	void setParamMinHeight(int height);

	~ProcedureInput();

	QDateEdit* qDateEdit();

	virtual AbstractDateEdit* dateEdit();

	Ui::ProcedureInput ui;


	// Inherited via IProcedureInput
	void setCommonData(const CommonData& data, bool hasNhifCode) override;

	void setParameterData() override;
	void setParameterData(AnesthesiaMinutes minutes) override;
	void setParameterData(bool supernumeral) override;
	void setParameterData(bool supernumeral, RestorationData r) override;
	void setParameterData(ConstructionRange range, bool allowSingle) override;
	void setParameterData(bool supernumeral, ConstructionRange range, bool preferSingle) override;
	void setParameterData(bool supernumeral, ConstructionRange range, RestorationData r, int preferedIndex) override;

	ResultData getResult();

	void setErrorMsg(const std::string& errorMsg);
	void disablePost() override;


	std::string isValid() override;

};
