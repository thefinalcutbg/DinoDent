#pragma once

#include <QWidget>
#include "ui_RecipientEditView.h"
#include "Model/Financial/Recipient.h"

class RecipientEditView : public QDialog
{
	Q_OBJECT

	std::optional<Recipient> m_recipient;

	void paintEvent(QPaintEvent* event) override;

public:
	RecipientEditView(const Recipient& r, QWidget *parent = nullptr);
	~RecipientEditView();
	std::optional<Recipient> getResult() { return m_recipient; };

private:
	Ui::RecipientEditViewClass ui;
};
