#pragma once

#include <QLineEdit>
#include <QKeyEvent>

#include "../AbstractLineEdit.h"

class LineEdit : public QLineEdit, public AbstractLineEdit
{
	Q_OBJECT
	
	bool defaultWidthSet;
	int defaultWidth;
	void dynamicWidthChange();
	bool disabled;
protected:

	virtual void stateChangedByUser() override;
public:
	LineEdit(QWidget *parent);
	~LineEdit();

	void setFocusAndSelectAll() override;
	void setAppearence(bool valid) override;
	void setFieldText(const std::string& text) override;
	std::string getText() override;
	void disable(bool disable) override;
	void keyPressEvent(QKeyEvent* event) override;
};



