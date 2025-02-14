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
	setFixedHeight(49); //CONSTANT HEIGHT
}

void TabTitle::setData(const TabName& tabData)
{
	ui.header->setText(tabData.header.c_str());
	ui.footer->setText(tabData.footer.c_str());

	ui.his->setPixmap(CommonIcon::getPixmap(tabData.header_icon));
	ui.nhif->setPixmap(CommonIcon::getPixmap(tabData.footer_icon));
	ui.indicator->setPixmap(CommonIcon::getIndicator(tabData.indicatorColor.c_str()));

	ui.his->setHidden(ui.his->pixmapIsNull());
	ui.nhif->setHidden(ui.nhif->pixmapIsNull());
	ui.indicator->setHidden(ui.indicator->pixmapIsNull());


	QFontMetrics headerMetrics(ui.header->font());
	QFontMetrics footerMetrics(ui.footer->font());

	int padding = ui.nhif->isHidden() && ui.his->isHidden() ? 40 : 50;

	int footerPadding = ui.indicator->isHidden() ? 0 : 14;

	int width = std::max(
		headerMetrics.boundingRect(ui.header->text()).width(),
		footerMetrics.boundingRect(ui.footer->text()).width() + footerPadding
	) + padding;

	resize(width, height());

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

TabTitle::~TabTitle()
{
}
