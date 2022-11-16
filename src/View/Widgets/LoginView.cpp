#include "LoginView.h"
#include <QPainter>
#include "Presenter/LoginPresenter.h"
#include "PracticeListDialog.h"
#include "View/Theme.h"
LoginView::LoginView(LoginPresenter* p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Вход");

	ui.practiceButton->setCursor(QCursor(Qt::PointingHandCursor));
	ui.practiceButton->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoiseClicked));
	//ui.rememberCheck->setStyleSheet("color:" + Theme::colorToString(Theme::fontTurquoise));
	ui.passEdit->setEchoMode(QLineEdit::Password);

	presenter->setView(this);

	connect(ui.practiceButton, &QPushButton::clicked, [=] {presenter->practiceListPressed();});
	connect(ui.okButton, &QPushButton::clicked, [=]{

				presenter->okPressed(
						ui.userEdit->text().toStdString(),
						ui.passEdit->text().toStdString(),
						ui.rememberCheck->isChecked()
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
