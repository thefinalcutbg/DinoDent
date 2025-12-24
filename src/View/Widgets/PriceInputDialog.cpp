#include "PriceInputDialog.h"
#include "ui_PriceInputDialog.h"

#include "View/GlobalFunctions.h"

PriceInputDialog::PriceInputDialog(const std::pair<double, double> &priceRange, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::PriceInputDialog)
{
    ui->setupUi(this);

    setWindowTitle("Въвеждане на цена");

    connect(ui->rangeRadio, &QRadioButton::clicked, this, [this](bool checked){
        ui->fromSpin->setDisabled(!checked);
        ui->toSpin->setDisabled(!checked);
        ui->exactSpin->setDisabled(checked);
        ui->fromSpin->setFocus();
        ui->fromSpin->selectAll();
    });

    connect(ui->exactRadio, &QRadioButton::clicked, this, [this](bool checked){
        ui->fromSpin->setDisabled(checked);
        ui->toSpin->setDisabled(checked);
        ui->exactSpin->setDisabled(!checked);
        ui->exactSpin->setFocus();
        ui->exactSpin->selectAll();
    });

    connect(ui->okButton, &QPushButton::clicked, this, [this]{

        m_result = ui->exactRadio->isChecked() ?
        std::make_pair(ui->exactSpin->value(), ui->exactSpin->value())
        :
        std::make_pair(ui->fromSpin->value(), ui->toSpin->value());

        if(m_result->first > m_result->second){
            std::swap(m_result->first, m_result->second);
        }

        accept();
    });

    ui->exactSpin->setValue(priceRange.first);
    ui->fromSpin->setValue(priceRange.first);
    ui->toSpin->setValue(priceRange.second);

    ui->exactSpin->setSuffix(getPriceSuffix(2026));
    ui->fromSpin->setSuffix(getPriceSuffix(2026));
    ui->toSpin->setSuffix(getPriceSuffix(2026));

    if(priceRange.first == priceRange.second){
        ui->exactRadio->setChecked(true);
        ui->exactSpin->setFocus();
    }
    else {
        ui->rangeRadio->setChecked(true);
        ui->fromSpin->setFocus();
    }

    adjustSize();
}

PriceInputDialog::~PriceInputDialog()
{
    delete ui;
}
