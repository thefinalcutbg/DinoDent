#pragma once

#include <QDialog>
#include "ui_PracticeDialog.h"

#include "Model/Validators/LpkValidator.h"
#include "Model/Validators/RziValidaor.h"
#include "Model/Validators/PatientValidators.h"

class PracticeDialog : public QDialog
{
	Q_OBJECT

    NotEmptyValidator not_empty_validator;
    RziValidator rzi_validator;
    CityValidator city_validator;
    LpkValidator lpk_validator;

	void paintEvent(QPaintEvent* event) override;

public:

    struct Data {
        std::string name;
        std::string rzi;
        std::string address;
    };

    PracticeDialog(QWidget *parent = Q_NULLPTR);

    std::optional<Data> getData() {return m_data; }

	~PracticeDialog();

private:

    std::optional<Data> m_data;

	Ui::PracticeDialog ui;


};
