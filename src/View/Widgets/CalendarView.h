#pragma once

#include <QWidget>
#include "ui_CalendarView.h"
#include "Model/CalendarStructs.h"
#include <QDate>

class CalendarPresenter;

class CalendarView : public QWidget
{
    Q_OBJECT

    CalendarPresenter* presenter{ nullptr };

    void initTable();

    void paintEvent(QPaintEvent* event) override;

public:
    CalendarView(QWidget *parent = nullptr);
    void showCalendar(bool show);
    void setCalendarPresenter(CalendarPresenter* p);
    void setCalendarList(const std::vector<Calendar>& list, int currentCalendar);
    void setEventList(const std::vector<CalendarEvent>& list, const CalendarEvent& clipboard);
    void updateWeekView(QDate from, QDate to, int currentDayColumn);
    ~CalendarView();

private:
    Ui::CalendarView ui;
};
