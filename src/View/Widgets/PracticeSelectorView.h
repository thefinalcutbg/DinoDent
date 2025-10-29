#pragma once

#include <QDialog>
#include "ui_PracticeSelectorView.h"
#include "Presenter/PracticeManagerPresenter.h"

class PracticeSelectorView : public QDialog
{
	Q_OBJECT

    PracticeManagerPresenter& presenter;

	void paintEvent(QPaintEvent* e) override;

public:
    PracticeSelectorView(PracticeManagerPresenter& p, QWidget *parent = nullptr);
	void setPracticeList(const std::vector<std::string>& practiceList);
	void setDbPath(const std::string& dbPath);
	~PracticeSelectorView();

private:
	Ui::PracticeSelectorViewClass ui;

	// Inherited via IPracticeSelectorView

};
