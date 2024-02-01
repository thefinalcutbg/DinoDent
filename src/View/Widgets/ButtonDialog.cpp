#include "ButtonDialog.h"
#include <QPushButton>
#include <QPainter>

ButtonDialog::ButtonDialog(const std::vector<std::string>& buttonNames, const std::string& title)
{
	ui.setupUi(this);

	setWindowTitle(title.c_str());

	for (int i = 0; i < buttonNames.size(); i++)
	{
		QPushButton* button = new QPushButton(buttonNames[i].c_str(), this);
		
		connect(button, &QPushButton::clicked, [this, i]{ m_result = i; accept();});

		ui.buttonLayout->insertWidget(ui.buttonLayout->count()-1, button);
	}
}



void ButtonDialog::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

	painter.fillRect(0, 0, width(), height(), Qt::white);
}


ButtonDialog::~ButtonDialog()
{

}
