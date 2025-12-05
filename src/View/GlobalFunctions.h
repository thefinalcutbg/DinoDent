#pragma once
#include <QString>

class QTableView;

QString getPriceSuffix(int year);
QString priceToString(double price, int year);
QString roundDouble(double number);
QString formatDoubleWithDecimal(const double& price);
void setTableViewDefaults(QTableView* view);