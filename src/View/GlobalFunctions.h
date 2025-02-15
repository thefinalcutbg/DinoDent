#pragma once
#include <QString>

class QTableView;

QString priceToString(double price);
QString roundDouble(double number);
QString formatDoubleWithDecimal(const double& price);
void setTableViewDefaults(QTableView* view);