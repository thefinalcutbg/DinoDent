#include "TextBrowser.h"
#include <QMouseEvent>

TextBrowser::TextBrowser(QWidget* parent)
	: QTextBrowser(parent)
{
}

TextBrowser::~TextBrowser()
{}

void TextBrowser::mousePressEvent(QMouseEvent* e)
{

	if (e->button() == Qt::LeftButton)
	{
		auto str = anchorAt(e->pos());

		if (str.isEmpty()) return;

		emit linkPressed(str);

		return;
	}

	QTextBrowser::mousePressEvent(e);
}
