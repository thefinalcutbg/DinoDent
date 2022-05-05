#include "LoginView.h"
#include <QPainter>
#include "Presenter/LoginPresenter/LoginPresenter.h"
#include "PracticeListDialog.h"

LoginView::LoginView(LoginPresenter* p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle(u8"Вход в системата");

	ui.passEdit->setEchoMode(QLineEdit::Password);

	presenter->setView(this);

	connect(ui.okButton, &QPushButton::clicked, [=]{

				presenter->okPressed(

						ui.userEdit->text().toStdString(),
						ui.passEdit->text().toStdString()
				);

		}
	);

	//ui.userEdit->setText("220008771");
	//ui.passEdit->setText("198312");
	ui.userEdit->setFocus();
	
}

void LoginView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}

int LoginView::practiceUserChoice(const std::vector<std::string>& practiceList)
{
	PracticeListDialog p;
	return p.exec(practiceList);
}

void LoginView::closeLogin()
{
	close();
}

LoginView::~LoginView()
{
}
