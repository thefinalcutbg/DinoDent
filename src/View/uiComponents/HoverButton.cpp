#include "HoverButton.h"
#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QApplication>
#include <QVariantAnimation>
#include <QEasingCurve>
#include "View/Theme.h"

HoverButton::HoverButton(QWidget* parent)
	: QPushButton(parent)
{
	installEventFilter(this);

#ifdef Q_OS_MAC
	setStyle(Theme::fusionStyle());
#endif

	setCursor(QCursor(Qt::PointingHandCursor));

	m_hoverAnimation = new QVariantAnimation(this);
	m_hoverAnimation->setDuration(150);
	m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
	m_hoverAnimation->setStartValue(0.0);
	m_hoverAnimation->setEndValue(0.0);

	connect(m_hoverAnimation, &QVariantAnimation::valueChanged, this,
		[this](const QVariant& value) {
			m_hoverProgress = value.toReal();
			update();
		});
}


void HoverButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath outline;
	outline.addRoundedRect(1, 1, rect().width() - 2, rect().height() - 2, 10, 10);

	QColor normalBg(Theme::sectionBackground);
	QColor hoverBg(Theme::background);

	auto mixChannel = [](int a, int b, qreal t) -> int {
		return a + qRound((b - a) * t);
		};

	qreal t = m_hoverProgress;
	if (t < 0.0) t = 0.0;
	if (t > 1.0) t = 1.0;

	QColor background;
	background.setRed(mixChannel(normalBg.red(), hoverBg.red(), t));
	background.setGreen(mixChannel(normalBg.green(), hoverBg.green(), t));
	background.setBlue(mixChannel(normalBg.blue(), hoverBg.blue(), t));
	background.setAlpha(mixChannel(normalBg.alpha(), hoverBg.alpha(), t));

	painter.fillPath(outline, background);

	QString paintText = m_hover && m_hoverText.size() ? m_hoverText : text();

	QPen pen;
	pen.setColor(Theme::buttonFrame);
	pen.setCosmetic(true);
	painter.setPen(pen);
	painter.drawPath(outline);

	pen.setColor(Theme::fontTurquoise);
	painter.setPen(pen);
	painter.setFont(QFont());
	painter.drawText(rect(), Qt::AlignCenter, paintText);

	painter.end();
}

bool HoverButton::eventFilter(QObject*, QEvent* e)
{
	if (e->type() == QEvent::Enter) {
		m_hover = true;
	
		if (m_hoverAnimation) {
			m_hoverAnimation->stop();
			m_hoverAnimation->setStartValue(m_hoverProgress);
			m_hoverAnimation->setEndValue(1.0);
			m_hoverAnimation->start();
		}
		else {
			m_hoverProgress = 1.0;
			update();
		}
	}

	if (e->type() == QEvent::Leave) {
			m_hover = false;

		if (m_hoverAnimation) {
			m_hoverAnimation->stop();
			m_hoverAnimation->setStartValue(m_hoverProgress);
			m_hoverAnimation->setEndValue(0.0);
			m_hoverAnimation->start();
		}
		else {
			m_hoverProgress = 0.0;
			update();
		}
	}

	return false;
}


HoverButton::~HoverButton()
{
}
