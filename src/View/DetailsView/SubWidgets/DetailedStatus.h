#pragma once

#include <QWidget>
#include "IDetailedStatusView.h"
#include "ui_DetailedStatus.h"
#include "View/ListView/ToothPaintDevices/ToothPainter.h"
#include "Model/Tooth/Enums.h"

class DetailedStatus : public QWidget, public IDetailedStatusView
{
	Q_OBJECT

	ToothPainter painter;

	inline static QString statusNames[statusCount]
	{ u8"Временен зъб", u8"Обтурация", u8"Кариес",  u8"Пулпит", u8"Периодонтит",
	  u8"Ендодонтско лечение", u8"Радикуларен щифт", u8"Корен",u8"Фрактура", u8"Екстракция",
	  u8"Пародонтит", u8"Подвижност I", u8"Подвижност II", u8"Подвижност III",
	  u8"Корона", u8"Мост/Блок корони",  u8"Имплант", u8"Свръхброен зъб" };

	inline static QString surfName[surfaceCount]
	{ u8"Оклузално", u8"Медиално", u8"Дистално",
		u8"Букално", u8"Лингвално", u8"Цервикално" };

public:
	DetailedStatus(QWidget *parent = Q_NULLPTR);

	void setCheckModel(const CheckModel& checkModel) override;
	void disableItem(int index, bool disabled) override;
	void paintTooth(const ToothPaintHint& hint) override;

	~DetailedStatus();

signals:
	void selectionChanged(int category, int code);
	void itemChecked(bool checked);


private:
	Ui::DetailedStatus ui;




};
