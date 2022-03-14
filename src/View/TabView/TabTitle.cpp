#include "TabTitle.h"
#include "TabBar.h"
#include <QFontMetrics>
#include <algorithm>
#include "View/Theme.h"

TabTitle::TabTitle(TabBar* parent, int tabId)
	: QWidget(parent), tabId(tabId), m_parent(parent)
{
	ui.setupUi(this);

	int tabIndex = parent->addTab("");
	parent->setTabButton(tabIndex, TabBar::RightSide, this);

	connect(ui.pushButton, &QPushButton::clicked, this, [=] {
				m_parent->requestClose(this);
	});

	ui.header->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);
	ui.footer->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);
	
	auto font = ui.header->font();
	font.setBold(true);
	ui.header->setFont(font);

	font.setFamily("Segoe UI Semibold");
	ui.footer->setFont(font);

	font.setPointSize(10);
	ui.pushButton->setFont(font);
	//setFixedHeight(this->height());

}

void TabTitle::setText(const QString& header, const QString& footer)
{
	ui.header->setText(header);
	ui.footer->setText(footer);

	QFontMetrics headerMetrics(ui.header->font());
	QFontMetrics footerMetrics(ui.footer->font());

	int width = std::max(
		headerMetrics.boundingRect(ui.header->text()).width(),
		footerMetrics.boundingRect(ui.footer->text()).width()
	) + 30;

	resize(width, height());

	for (int i = 0; i < m_parent->count(); i++) {	//re-setting widget to tab view
		if (m_parent->tabButton(i, QTabBar::RightSide) == static_cast<QWidget*>(this))
		{
			m_parent->setTabButton(i, QTabBar::RightSide, nullptr);
			m_parent->setTabButton(i, QTabBar::RightSide, this);
		}
	}
}

void TabTitle::setCurrentAppearence(bool current)
{
	

	if (current) {
		setStyleSheet(
			"QLabel{color:" + Theme::getRGBStringFromColor(Theme::fontTurquoiseClicked) + "}"
			"QPushButton{color:" + Theme::getRGBStringFromColor(Theme::fontRed) + "background - color:transparent; } "
			"QPushButton:hover{color:" + Theme::getRGBStringFromColor(Theme::fontRedClicked) + "}"
			"QPushButton:pressed:hover{color: darkred;}"

		);
	}
	else
	{
		setStyleSheet(
			"QLabel{color:" + Theme::getRGBStringFromColor(Theme::fontTurquoise) + "}"
			"QPushButton{color:" + Theme::getRGBStringFromColor(Theme::fontTurquoise) + "background - color:transparent; } "
			"QPushButton:hover{color:" + Theme::getRGBStringFromColor(Theme::fontRedClicked) + "}"
			"QPushButton:pressed:hover{color: darkred;}"
		);
	}
}

TabTitle::~TabTitle()
{
}
