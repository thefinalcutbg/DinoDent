#pragma once

#include <QTextBrowser>

class TextBrowser : public QTextBrowser
{
	Q_OBJECT

		void mousePressEvent(QMouseEvent* e) override;

public:
	TextBrowser(QWidget* parent);
	~TextBrowser();

signals:
	void linkPressed(const QString& str);

};