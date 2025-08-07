#include "IconButton.h"
#include <QPainter>
#include <QIcon>
#include <QEvent>
#include <QPainterPath>
#include <QStatusBar>
#include <QApplication>
#include "View/Theme.h"
#include "View/Widgets/GlobalWidgets.h"

IconButton::IconButton(QWidget *parent)
	: 
	QPushButton(parent), 
	hoverColor(Theme::background), 
	backgroundColor(Theme::sectionBackground)
{
	this->installEventFilter(this);

#ifdef Q_OS_MAC
    setStyle(Theme::fusionStyle());
#endif

}

void IconButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);

	QPainterPath path;
	path.addEllipse(rect());

	QColor color = isEnabled() && (m_hover || isChecked()) ?
			hoverColor : backgroundColor ;

	painter.fillPath(path, color);
	
	int padding = m_clicked && m_hover ? 15 : 12;

	QRect iconRect(rect().center(), QSize(rect().width()-padding, rect().height()-padding));
	iconRect.moveCenter(rect().center());
	
	QIcon::Mode iconMode = isEnabled() ? QIcon::Mode::Normal : QIcon::Mode::Disabled;

	if (m_monochrome) {
		iconMode = QIcon::Mode::Disabled;
	}

	if (!icon().isNull()) {
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		icon().paint(&painter, iconRect, Qt::AlignCenter, iconMode);
		painter.setRenderHint(QPainter::Antialiasing, true);
	}

	if (outlineColor.isValid()) {
		QPen pen(outlineColor);
		pen.setCosmetic(true);
		pen.setWidth(1);
		painter.setPen(pen);
		painter.drawEllipse(rect());
	}
	
}

bool IconButton::eventFilter(QObject*, QEvent* e)
{
    if (e->type() == QEvent::Enter) {

		m_hover = true;
		GlobalWidgets::statusBar->showMessage(this->toolTip());

		if (isEnabled()) {
			QApplication::setOverrideCursor(Qt::PointingHandCursor);
		}

		update();
	}

    if (e->type() == QEvent::Leave) {
		m_hover = false;
		GlobalWidgets::statusBar->clearMessage();
		QApplication::restoreOverrideCursor();
		update();
	}

	if (e->type() == QEvent::MouseButtonPress)
	{
		m_clicked = true;
	}

	if (e->type() == QEvent::MouseButtonRelease)
	{
		m_clicked = false;
	}

	return false;
}


void IconButton::setHoverColor(const QColor& color)
{
	hoverColor = color;
	update();
}

void IconButton::setBackgroundColor(const QColor& color)
{
	backgroundColor = color;
	update();
}

void IconButton::setMonochrome(bool monochrome)
{
	m_monochrome = monochrome;
	update();
}

IconButton::~IconButton()
{
}
