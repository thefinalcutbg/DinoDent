#include "StatusButton.h"
#include "View/Theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QMouseEvent>
#include <QVariantAnimation>
#include <QEasingCurve>

StatusButton::StatusButton(QWidget* parent)
	: QPushButton(parent), checkState(CheckState::unchecked)
{
	setCheckable(true);
	setFocusPolicy(Qt::NoFocus);
	installEventFilter(this);

	m_checkAnimation = new QVariantAnimation(this);
	m_checkAnimation->setDuration(150);
	m_checkAnimation->setEasingCurve(QEasingCurve::OutCubic);
	m_checkAnimation->setStartValue(0.0);
	m_checkAnimation->setEndValue(0.0);

	connect(m_checkAnimation, &QVariantAnimation::valueChanged, this,
		[this](const QVariant& value) {
			m_checkProgress = value.toReal();
			update();
		});

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

	connect(this, &QPushButton::toggled, this,
		[this](bool checked) {
			if (!m_checkAnimation)
				return;
			m_checkAnimation->stop();
			m_checkAnimation->setStartValue(m_checkProgress);
			m_checkAnimation->setEndValue(checked ? 1.0 : 0.0);
			m_checkAnimation->start();
		});
}

void StatusButton::setCheckState(const CheckState& checkState)
{
	this->checkState = checkState;

	if (checkState == CheckState::checked) {
		setChecked(true);
		return;
	}

	setChecked(false);
}

void StatusButton::paintEvent(QPaintEvent*)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QPainterPath outline;
	outline.addRoundedRect(1, 1, rect().width() - 2, rect().height() - 2, 10, 10);

	auto mixChannel = [](int a, int b, qreal t) -> int {
		return a + qRound((b - a) * t);
		};

	qreal h = m_hoverProgress;
	if (h < 0.0) h = 0.0;
	if (h > 1.0) h = 1.0;

	QColor normalBg = Theme::sectionBackground;
	QColor hoverBg = Theme::background;

	QColor background;
	background.setRed(mixChannel(normalBg.red(), hoverBg.red(), h));
	background.setGreen(mixChannel(normalBg.green(), hoverBg.green(), h));
	background.setBlue(mixChannel(normalBg.blue(), hoverBg.blue(), h));
	background.setAlpha(mixChannel(normalBg.alpha(), hoverBg.alpha(), h));

	painter.fillPath(outline, background);

	qreal t = m_checkProgress;
	if (t < 0.0) t = 0.0;
	if (t > 1.0) t = 1.0;

	QColor uncheckedOutline = Theme::buttonFrame;
	QColor uncheckedText = Theme::fontTurquoise;
	QColor checkedOutline = pathology ? Theme::fontRed : Theme::fontTurquoiseClicked;
	QColor checkedText = checkedOutline;

	QColor outlineColor;
	outlineColor.setRed(mixChannel(uncheckedOutline.red(), checkedOutline.red(), t));
	outlineColor.setGreen(mixChannel(uncheckedOutline.green(), checkedOutline.green(), t));
	outlineColor.setBlue(mixChannel(uncheckedOutline.blue(), checkedOutline.blue(), t));
	outlineColor.setAlpha(mixChannel(uncheckedOutline.alpha(), checkedOutline.alpha(), t));

	QColor textColor;
	textColor.setRed(mixChannel(uncheckedText.red(), checkedText.red(), t));
	textColor.setGreen(mixChannel(uncheckedText.green(), checkedText.green(), t));
	textColor.setBlue(mixChannel(uncheckedText.blue(), checkedText.blue(), t));
	textColor.setAlpha(mixChannel(uncheckedText.alpha(), checkedText.alpha(), t));

	QPen pen;
	pen.setColor(outlineColor);
	painter.setPen(pen);
	painter.drawPath(outline);

	QFont font;
	if (isChecked())
		font.setBold(true);

	painter.setFont(font);
	pen.setColor(textColor);
	painter.setPen(pen);
	painter.drawText(rect(), Qt::AlignCenter, text());

	painter.end();
}

bool StatusButton::eventFilter(QObject*, QEvent* e)
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

	if (e->type() == QEvent::MouseButtonPress) {
		m_clicked = true;
	}

	if (e->type() == QEvent::MouseButtonRelease) {
		m_clicked = false;
	}

	return false;
}

void StatusButton::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {

		if (menu()) {
			showMenu();
			m_hover = false;
			update();
			return;
		}

		emit clicked();
		return;
	}

	if (e->button() == Qt::RightButton) {
		emit rightClicked();
	}

	QPushButton::mousePressEvent(e);
}
