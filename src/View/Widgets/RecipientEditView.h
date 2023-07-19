#pragma once

#include <QWidget>
#include "ui_RecipientEditView.h"
#include "Model/Financial/Recipient.h"
#include "Presenter/RecipientPresenter.h"

class RecipientEditView : public QDialog
{
	Q_OBJECT

	std::optional<Recipient> m_recipient;

	void paintEvent(QPaintEvent* event) override;

	RecipientPresenter& presenter;

public:

	enum LineEditType { ID, Name, Address, Phone };

	RecipientEditView(RecipientPresenter& presenter, QWidget *parent = nullptr);

	void setRecipient(const Recipient& r);
	Recipient getRecipient();

	std::optional<Recipient> getResult() { return m_recipient; };
	LineEdit* lineEdit(LineEditType type);

	~RecipientEditView();

private:

	LineEdit* lineEdits[4] { nullptr };

	Ui::RecipientEditViewClass ui;
};
