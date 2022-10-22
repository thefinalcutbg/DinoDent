#pragma once

#include <QDialog>
#include "ui_PracticeSelectorView.h"
#include "View/Interfaces/IPracticeSelectorView.h"
#include "Presenter/PracticeManagerPresenter.h"

class PracticeSelectorView : public QDialog, public IPracticeSelectorView
{
	Q_OBJECT

	PracticeManagerPresenter* presenter;

	void paintEvent(QPaintEvent* e) override;

public:
	PracticeSelectorView(PracticeManagerPresenter* p, QWidget *parent = nullptr);
	void setPracticeList(const std::vector<std::string>& practiceList) override;
	void setDbPath(const std::string& dbPath) override;
	void closeDialog() override  {close();}
	~PracticeSelectorView();

private:
	Ui::PracticeSelectorViewClass ui;

	// Inherited via IPracticeSelectorView

};
