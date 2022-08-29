#pragma once

#include <QDialog>
#include "ui_PinPromptDialog.h"
#include <string>
class PinPromptDialog : public QDialog
{
	Q_OBJECT

	QString result;

	void paintEvent(QPaintEvent* event) override;

public:
	PinPromptDialog(const std::string& pem, QWidget *parent = Q_NULLPTR);
	~PinPromptDialog();

	const QString& getResult() const { return result; }

private:
	Ui::PinPromptDialog ui;
};
