#pragma once

#include <QWidget>
#include "ui_ReferralTile.h"

class Referral;

class ReferralTile : public QWidget
{
	Q_OBJECT

	void paintEvent(QPaintEvent* e) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

	bool m_hover{ false };
	bool m_clicked{ false };

	int m_index;

public:
	ReferralTile(const Referral& referral, int index, QWidget *parent = nullptr);
	~ReferralTile();

signals:
	void clicked(int index);
	void print(int index);
	void remove(int index);

private:
	Ui::ReferralTileClass ui;
};
