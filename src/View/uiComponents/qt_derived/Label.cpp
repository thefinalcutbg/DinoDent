#include "Label.h"

Label::Label(QWidget *parent)
	: QLabel(parent)
{
}

void Label::setLabelText(const std::string& text)
{
	setText(QString::fromStdString(text));
}

Label::~Label()
{
}
