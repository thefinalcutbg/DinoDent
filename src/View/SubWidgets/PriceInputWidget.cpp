#include "PriceInputWidget.h"
#include "ui_PriceInputWidget.h"
#include "View/GlobalFunctions.h"

PriceInputWidget::PriceInputWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PriceInputWidget)
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
}

void PriceInputWidget::setPrice(const std::pair<double, double> &priceRange)
{

    ui->exactSpin->setValue(priceRange.first);
    ui->fromSpin->setValue(priceRange.first);
    ui->toSpin->setValue(priceRange.second);

    ui->exactSpin->setSuffix(getPriceSuffix(2026));
    ui->fromSpin->setSuffix(getPriceSuffix(2026));
    ui->toSpin->setSuffix(getPriceSuffix(2026));

    if(priceRange.first == priceRange.second){
        ui->exactRadio->click();
    }
    else {
        ui->rangeRadio->click();
    }

}

std::pair<double, double> PriceInputWidget::getPrice()
{
    decltype(getPrice()) result;

    result = ui->exactRadio->isChecked() ?
                 std::make_pair(ui->exactSpin->value(), ui->exactSpin->value())
                                         :
                 std::make_pair(ui->fromSpin->value(), ui->toSpin->value());

    if(result.first > result.second){
        std::swap(result.first, result.second);
    }

    return result;
}

PriceInputWidget::~PriceInputWidget()
{
    delete ui;
}
