#pragma once

#include <QDialog>
#include "ui_LoginView.h"
#include "View/Interfaces/ILoginView.h"

class LoginPresenter;

class LoginView : public QDialog, public ILoginView
{
	Q_OBJECT

	void paintEvent(QPaintEvent* event) override;
    LoginPresenter& presenter;

	
public:
    LoginView(LoginPresenter& p, QWidget *parent = Q_NULLPTR);
	int practiceUserChoice(const std::vector<std::string>& practiceList) override; //-1 indicates user has closed the dialog
	void closeLogin() override;
	~LoginView();

private:
	Ui::LoginView ui;
};
