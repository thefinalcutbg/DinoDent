#pragma once

#include <QPushButton>


class HalfRoundedButton : public QPushButton
{
	Q_OBJECT

public:
	enum class Position { Right, Center, Left };

	void setPosition(Position p) {m_position = p; update();}
	HalfRoundedButton(QWidget* parent);
;
	~HalfRoundedButton();

private:

	bool m_hover{ false };

	Position m_position{ Position::Left };
	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;
};
