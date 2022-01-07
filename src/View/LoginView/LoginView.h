#pragma once

#include <QDialog>
#include "ui_LoginView.h"
#include "ILoginView.h"

class LoginPresenter;

class LoginView : public QDialog, public ILoginView
{
	Q_OBJECT

	void paintEvent(QPaintEvent* event) override;
	LoginPresenter* presenter;

	
public:
	LoginView(LoginPresenter* p, QWidget *parent = Q_NULLPTR);
	void setPracticeNames(const std::vector<std::string>& practiceList) override;
	void closeLogin() override;
	~LoginView();

private:
	Ui::LoginView ui;
};
