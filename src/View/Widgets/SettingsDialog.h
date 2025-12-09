#pragma once

#include <QDialog>

#include "ui_SettingsDialog.h"

#include "Presenter/SettingsMainPresenter.h"

#include "View/TableModels/SpecialtyTableModel.h"
#include "View/TableModels/PlainTableModel.h"

#include "Model/Settings.h"
#include "Model/GlobalSettingsData.h"
#include "Model/UserStructs.h"
#include "Model/DynamicNum.h"

#include "Model/Validators/BulstatValidator.h"
#include "Model/Validators/CommonValidators.h"
#include "Model/Validators/LpkValidator.h"
#include "Model/Validators/NameValidator.h"
#include "Model/Validators/NzokContractValidator.h"
#include "Model/Validators/RziValidaor.h"
#include "Model/Validators/PatientValidators.h"

class SettingsDialog : public QDialog
{
	Q_OBJECT

	SettingsMainPresenter presenter;

	NotEmptyValidator not_empty_validator;
	BulstatValidator bulstat_validator;
	IbanValidator iban_validator;
	BICValidator bic_validator;
	LpkValidator lpk_validator;
	NameValidator name_validator;
	NzokContractValidator nhif_contract_validator;
	RziValidator rzi_validator;
	EgnLnchValidator grao_validator;
	CityValidator city_validator;

	SpecialtyTableModel his_specialtyModel;
	PlainTableModel sql_table_model;

	std::vector<DirType> dir_structure;


	constexpr static inline int specIdxSize = 5;

	constexpr static inline const char* specialties[specIdxSize]{
	"Не е посочен",
	"64 - Общопрактикуващ лекар по дентална медицина",
	"61 - Детски лекар по дентална медицина",
	"62 - Орална хирургия",
	"68 - Дентална, орална и лицево-челюстна хирургия"
	};

	static inline const std::vector<std::string> s_dirStr = {
		"Номер на практика",
		"УИН на лекар",
		"Пациент (ФАМИЛИЯ, Име)",
		"Пациент (Име, Фамилия)",
		"Година-Месец",
		"Вид на документа"
	};

	void disableNhifValidators(bool disabled);
	void legalEntityChanged(bool selfInsured);
    QString getDoctorName(const PracticeDoctor& entity);
	void refreshDirStructureUI();
//	void paintEvent(QPaintEvent* event) override;

public:

	enum class Tab { General, PKCS, PenTablet, SMS, Doctor, Practice, Company, NhifContract, CodeList, SQL };

	SettingsDialog(QDialog *parent = Q_NULLPTR);

    void focusTab(Tab tab);
    void setAdminPriv(bool admin);

	void setSettings(const Settings& settings);
	Settings getSettings();
	void setUpdateDate(DynamicNum num, const Date& date);

    void setGlobalSettings(const GlobalSettingsData& data);
	GlobalSettingsData getGlobalSettings();

	ProcedureListView* getPriceListView();
	void setSqlTable(const PlainTable& table);

	void setPractice(const Practice& practice);
	void setDoctor(const Doctor& doctor);
	void setDoctorList(const std::vector<PracticeDoctor>& doctors);
	void setDoctorProperties(bool admin, NhifSpecialty specialty);
	Practice getPractice();
	Doctor getDoctor();
	void replaceCurrentItem(const PracticeDoctor& item);
	bool allFieldsAreValid();

	~SettingsDialog();

private:
	Ui::SettingsDialog ui;
};
