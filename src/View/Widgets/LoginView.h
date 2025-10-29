#pragma once

#include <QDialog>
#include "ui_LoginView.h"

class LoginPresenter;

class LoginView : public QDialog
{
	Q_OBJECT

	void paintEvent(QPaintEvent* event) override;
    LoginPresenter& presenter;

public:
    LoginView(LoginPresenter& p, QWidget *parent = Q_NULLPTR);
	void setDoctorList(const std::vector <std::string>& doctorList);
	void disablePasswordField(bool disabled);
	int practiceUserChoice(const std::vector<std::string>& practiceList); //-1 indicates user has closed the dialog
	~LoginView();

private:
	Ui::LoginView ui;
};
