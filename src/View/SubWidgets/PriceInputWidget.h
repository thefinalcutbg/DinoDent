#pragma once

#include <QWidget>

namespace Ui {
class PriceInputWidget;
}

class PriceInputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PriceInputWidget(QWidget *parent = nullptr);

    void setPrice(const std::pair<double, double>& pice);

    std::pair<double, double> getPrice();

    ~PriceInputWidget();

private:
    Ui::PriceInputWidget *ui;
};
