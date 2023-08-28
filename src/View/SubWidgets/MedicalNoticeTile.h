#pragma once

#include <QWidget>
#include "ui_MedicalNoticeTile.h"

struct MedicalNotice;

class MedicalNoticeTile : public QWidget
{
	Q_OBJECT

	void paintEvent(QPaintEvent* e) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

	bool m_hover{ false };
	bool m_clicked{ false };

	int m_index;

public:
	MedicalNoticeTile(const MedicalNotice& notice, int index, QWidget* parent = nullptr);
	~MedicalNoticeTile();

signals:
	void clicked(int index);
	void remove(int index);
	void sendToHis(int index);

private:
	Ui::MedicalNoticeTileClass ui;
};
