#pragma once

#include <QWidget>
#include "ui_ProcedureInput.h"
#include "Model/Validators/CommonValidators.h"
#include "Model/Dental/Procedure.h"
#include "View/TableModels/MKBModel.h"

class DateEdit;

class ProcedureCreator;

class ProcedureInput : public QWidget
{
	Q_OBJECT

	ProcedureCreator* presenter{ nullptr };

	bool m_postDisabled = false;

	bool m_allow_singleRange = false;

	//financing combo logic
	void initFinancingCombo(bool hasNhifCode);
	void setFinancingSource(FinancingSource source);
	FinancingSource getFinancingSource();

	ConstructionRange getConstructionRange();
	RestorationData getRestorationData();


public:

	struct CommonData {

		Diagnosis diagnosis;
		FinancingSource financingSource{ FinancingSource::None };
		std::string notes;
		double value;
	};

	struct ResultData : public CommonData {
		std::variant<
			std::monostate, //no parameters
			int, //anesthesia minutes
			bool, //supernumeral for tooth specific
			std::pair<bool, RestorationData>, //restoration
			ConstructionRange //range
		> parameters;
	};


	ProcedureInput(QWidget *parent = Q_NULLPTR);

	void setParamMinHeight(int height);

	~ProcedureInput();

	QDateEdit* qDateEdit();

	virtual AbstractDateEdit* dateEdit();

	Ui::ProcedureInput ui;


	// Inherited via IProcedureInput
	void setCommonData(const CommonData& data, bool hasNhifCode);

	void setParameterData();
	void setParameterData(AnesthesiaMinutes minutes);
	void setParameterData(bool supernumeral);
	void setParameterData(bool supernumeral, RestorationData r);
	void setParameterData(ConstructionRange range, bool allowSingle);
	void setParameterData(bool supernumeral, ConstructionRange range, bool preferSingle);
	void setParameterData(bool supernumeral, ConstructionRange range, RestorationData r, int preferedIndex);
	void setCurrentPresenter(ProcedureCreator* presenter) { this->presenter = presenter; }

	ResultData getResult();

	void setErrorMsg(const std::string& errorMsg);
	void disablePost();


	std::string isValid();

};
