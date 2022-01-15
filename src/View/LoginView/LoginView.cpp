#include "LoginView.h"
#include <QPainter>
#include "Presenter/LoginPresenter/LoginPresenter.h"

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
						ui.passEdit->text().toStdString(),
						ui.practiceCombo->currentIndex()
				);

		}
	);

	connect(ui.cancelButton, &QPushButton::clicked, [=] { close(); });

	ui.userEdit->setFocus();
	
}

void LoginView::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	painter.end();
}

void LoginView::setPracticeNames(const std::vector<std::string>& practiceList)
{
	ui.practiceCombo->clear();

	for (auto& entity : practiceList)
		ui.practiceCombo->addItem(QString::fromStdString(entity));
		

}

void LoginView::closeLogin()
{
	close();
}

LoginView::~LoginView()
{
}
