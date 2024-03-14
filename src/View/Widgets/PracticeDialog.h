#pragma once

#include <QDialog>
#include "ui_PracticeDialog.h"
#include "View/Interfaces/IPracticeDialog.h"

#include "Model/Validators/LpkValidator.h"
#include "Model/Validators/RziValidaor.h"
#include "Model/Validators/PatientValidators.h"

class PracticeDialog : public QDialog, public IAddPracticeDialog
{
	Q_OBJECT

    NotEmptyValidator not_empty_validator;
    RziValidator rzi_validator;
    CityValidator city_validator;
    LpkValidator lpk_validator;

    std::optional<Data> m_data;

	void paintEvent(QPaintEvent* event) override;

public:
    PracticeDialog(QWidget *parent = Q_NULLPTR);

    virtual std::optional<Data> getData() override {return m_data; }

	~PracticeDialog();

private:
	Ui::PracticeDialog ui;


};
