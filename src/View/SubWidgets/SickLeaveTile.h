#pragma once

#include <QWidget>
#include "ui_SickLeaveTile.h"
#include "Model/SickLeave.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SickLeaveTileClass; };
QT_END_NAMESPACE

class SickLeaveTile : public QWidget
{
	Q_OBJECT

	void paintEvent(QPaintEvent* e) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

	bool m_hover{ false };
	bool m_clicked{ false };

	int m_index;

public:
	SickLeaveTile(const SickLeave& notice, int index, QWidget *parent = nullptr);
	~SickLeaveTile();

signals:
	void clicked(int index);
	void print(int index);
	void remove(int index);
	void sendToNSSI(int index);

private:
	Ui::SickLeaveTileClass *ui;
};

