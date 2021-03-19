#pragma once

#include <QLabel>

#include "AbstractLabel.h"

class Label : public QLabel, public AbstractLabel
{

public:
	Label(QWidget *parent);
	void setLabelText(const std::string& text);
	~Label();
};
