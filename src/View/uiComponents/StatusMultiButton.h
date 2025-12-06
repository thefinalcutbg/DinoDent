#pragma once

#include <QPushButton>
#include <vector>

class QVariantAnimation;

class StatusMultiButton : public QPushButton
{
	Q_OBJECT

	void paintEvent(QPaintEvent* e) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

	int m_state{ 0 };
	bool m_hover{ false };
	std::vector<QString> m_stateNames;

	qreal m_checkProgress{ 0.0 };
	QVariantAnimation* m_checkAnimation{ nullptr };
	qreal m_hoverProgress{ 0.0 };
	QVariantAnimation* m_hoverAnimation{ nullptr };

public:
	StatusMultiButton(QWidget* parent);

	bool pathology{ false };

	void setStateNames(const std::vector<QString>& names);
	void setCurrentState(int state);
	~StatusMultiButton() {};

signals:
	void stateChanged(int state);
};
