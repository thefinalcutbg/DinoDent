#pragma once

#include <QDoubleSpinBox>
#include "../AbstractSpinBox.h"

class SpinBox : public QDoubleSpinBox, public AbstractSpinBox
{
	Q_OBJECT

	bool disabled;

public:

	SpinBox(QWidget *parent);
	~SpinBox();

	// Inherited via AbstractSpinBox
	virtual void setFocus() override;
	virtual void disable(bool disable) override;
	virtual void setValidAppearence(bool valid) override;
	virtual void set_Value(double value) override;
	virtual double get_Value() override;
};
