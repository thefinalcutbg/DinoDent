#pragma once

#include <QWidget>
#include "ui_PisImportView.h"
#include "Presenter/PisImportPresenter.h"

class PisImportView : public QWidget
{
	Q_OBJECT

	PisImportPresenter presenter;

public:
	PisImportView(QWidget *parent = nullptr);

	void setProgresSize(int size);
	void incrementProgress();
	void clearConsole();
	void logToConsole(const std::string& text);
	void disableButton(bool disabled);
	~PisImportView();

private:
	Ui::PisImportViewClass ui;
};
