#include "TabTitle.h"
#include "View/Widgets/TabView.h"
#include <QFontMetrics>
#include <algorithm>
#include "View/Theme.h"

TabTitle::TabTitle(TabView* tabView, int tabId)
    : m_id(tabId), tabId(tabId)
{
	ui.setupUi(this);

	setCurrentAppearence(false);

	connect(ui.pushButton, &QPushButton::clicked, this, [=] {
				tabView->requestClose(tabId);
	});

	ui.header->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);
	ui.footer->setAttribute(Qt::WidgetAttribute::WA_TransparentForMouseEvents);
	
	auto font = ui.header->font();
	font.setBold(true);
	ui.header->setFont(font);

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

	int padding = ui.nhif->isHidden() && ui.his->isHidden() ? 40 : 50;

	int width = std::max(
		headerMetrics.boundingRect(ui.header->text()).width(),
		footerMetrics.boundingRect(ui.footer->text()).width()
	) + padding;

	resize(width, height());
	
	/*
	for (int i = 0; i < m_parent->count(); i++) {	//re-setting widget to tab view
		if (m_parent->tabButton(i, QTabBar::RightSide) == static_cast<QWidget*>(this))
		{
			m_parent->setTabButton(i, QTabBar::RightSide, nullptr);
			m_parent->setTabButton(i, QTabBar::RightSide, this);
		}
	}
	*/
}

void TabTitle::setCurrentAppearence(bool current)
{

	if (current) {
		setStyleSheet(
			"QLabel{color:" + Theme::colorToString(Theme::fontTurquoise) + "}"
            "QPushButton{color:" + Theme::colorToString(Theme::fontRed) + "background-color:transparent; } "
			"QPushButton:hover{color:" + Theme::colorToString(Theme::fontRedClicked) + "}"
            "QPushButton:pressed:hover{color: darkred; background-color:transparent;}"

		);
	}
	else
	{
		setStyleSheet(
			"QLabel{color:" + Theme::colorToString(Theme::fontTurquoiseClicked) + "}"
            "QPushButton{color:" + Theme::colorToString(Theme::fontTurquoiseClicked) + "background-color:transparent; } "
			"QPushButton:hover{color:" + Theme::colorToString(Theme::fontRedClicked) + "}"
            "QPushButton:pressed:hover{color: darkred; background-color:transparent;}"
		);
	}
}

void TabTitle::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton)
	{
		ui.pushButton->click();
		return;
	}

	QWidget::mouseReleaseEvent(event);
}

void TabTitle::setFooterIcon(const QPixmap& px)
{
	ui.nhif->setHidden(px.isNull());

    if(px.isNull()){ return; }

	ui.nhif->setPixmap(px);

}

void TabTitle::setHeaderIcon(const QPixmap& px)
{
	ui.his->setHidden(px.isNull());

    if(px.isNull()){ return; }
	
	ui.his->setPixmap(px);
}

TabTitle::~TabTitle()
{
}
