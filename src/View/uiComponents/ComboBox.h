#pragma once

#include <QComboBox>

#include "AbstractComboBox.h"


class ComboBox : public QComboBox, public AbstractComboBox
{
	Q_OBJECT

public:
	ComboBox(QWidget *parent);
	~ComboBox();

	int getIndex();
	void setIndex(int index);
	void disable(bool disable);
};
