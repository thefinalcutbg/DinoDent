#pragma once

#include <QDialog>

#include <optional>

namespace Ui {
class PriceInputDialog;
}

class PriceInputDialog : public QDialog
{
    Q_OBJECT

    std::optional<std::pair<double, double>> m_result;

public:
    explicit PriceInputDialog(const std::pair<double, double>& priceRange = {0,0}, QWidget *parent = nullptr);

    auto getResult(){ exec(); return m_result;}

    ~PriceInputDialog();

private:
    Ui::PriceInputDialog *ui;
};
