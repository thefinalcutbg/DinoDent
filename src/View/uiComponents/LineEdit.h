#pragma once

#include <QLineEdit>
#include "View/Interfaces/AbstractLineEdit.h"

class QLabel;

class LineEdit : public QLineEdit, public AbstractLineEdit
{
	Q_OBJECT
	
	bool defaultWidthSet;
	int defaultWidth;
	void dynamicWidthChange();
	bool disabled;
	QLabel* errorLabel;
protected:

public:
	LineEdit(QWidget *parent);
	~LineEdit();

	void setErrorLabel(QLabel* errorLabel);

	void AbstractUIElement::setFocus() override;
	void setValidAppearence(bool valid) override;
	void set_Text(const std::string& text) override;
	std::string getText() override;
	void disable(bool disable) override;
	void keyPressEvent(QKeyEvent* event) override;
};



