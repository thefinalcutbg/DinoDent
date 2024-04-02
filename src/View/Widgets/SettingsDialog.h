#pragma once

#include <QDialog>
#include "ui_SettingsDialog.h"

#include "Presenter/SettingsMainPresenter.h"

#include "View/Interfaces/ISettingsDialog.h"
#include "View/TableModels/SpecialtyTableModel.h"

#include "Model/Validators/BulstatValidator.h"
#include "Model/Validators/CommonValidators.h"
#include "Model/Validators/LpkValidator.h"
#include "Model/Validators/NameValidator.h"
#include "Model/Validators/NzokContractValidator.h"
#include "Model/Validators/RziValidaor.h"
#include "Model/Validators/PatientValidators.h"


class SettingsDialog : public QDialog, public ISettingsDialog
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
	
	void disableNhifValidators(bool disabled);
	void legalEntityChanged(bool selfInsured);

	void paintEvent(QPaintEvent* event) override;

public:
	SettingsDialog(QDialog *parent = Q_NULLPTR);

    void focusTab(SettingsTab tab) override;
    void disableTab(SettingsTab tab) override;
	void closeDialog() final { this->close(); };

	void setSettings(const Settings& settings) final;
	Settings getSettings() final;
	void setUpdateDate(DynamicNum num, const Date& date) final;
	virtual void setPkcs11List(const std::vector<std::string>& list) final;
	std::vector<std::string> getPkcs11List() final;
	void setDebug(bool showRequests, bool showReplies) final;
	bool showRequests() final;
	bool showReplies() final;
	void setDevBranch(bool dev) final;
	bool devBranch() final;
	~SettingsDialog();

private:
	Ui::SettingsDialog ui;

	// Inherited via ISettingsDialog
	virtual void setPractice(const Practice& practice) override;
	virtual void setDoctor(const Doctor& doctor) override;
	virtual void setDoctorList(const std::vector<PracticeDoctor>& doctors) override;
	virtual void setDoctorProperties(bool admin, NhifSpecialty specialty) override;
	virtual Practice getPractice() override;
	virtual Doctor getDoctor() override;
	virtual void replaceCurrentItem(const PracticeDoctor& item) override;

	// Inherited via ISettingsDialog
	virtual bool allFieldsAreValid() override;
};
