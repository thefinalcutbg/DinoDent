#pragma once

#include <QCalendarWidget>

class QPushButton;
class QVBoxLayout;
class QGridLayout;

class CalendarWidget : public QCalendarWidget
{

    Q_OBJECT

    enum menuContent {YEAR, MONTH, DAY};

    menuContent type {menuContent::DAY};
    QPushButton * prevYearButton;
    QPushButton * nextYearButton;
    QPushButton * prevMonthButton;
    QPushButton * nextMonthButton;
    QPushButton * yearButton;
    QPushButton * monthButton;
    QVBoxLayout * vBodyLayout;
    QGridLayout * monthGridLayout;
    QGridLayout * yearGridLayout;
    QWidget * monthMenu;
    QWidget * yearMenu;


public:
    CalendarWidget(QWidget* parent = nullptr);

private:

    bool eventFilter(QObject *widget, QEvent *event) override;

    void selectedMonth(int year, int month);
    void setDateLabelText(int year, int month);
    void selectedYear(int yearCount);
    void monthMenuPopup();
    void yearMenuPopup();
    void menuChanged();
    void changeMenu(int sequence);
    void changeType(menuContent type);
};


