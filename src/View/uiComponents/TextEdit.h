#pragma once

#include <QTextEdit>
#include <QKeyEvent>
#include "AbstractLineEdit.h"

class TextEdit : public QTextEdit, public AbstractLineEdit
{
	Q_OBJECT

public:
	TextEdit(QWidget *parent);
	~TextEdit();

	// Inherited via AbstractLineEdit
	virtual void setFocusAndSelectAll() override;
	virtual void disable(bool disable) override;
	virtual void setAppearence(bool valid) override;
	virtual void setFieldText(const std::string& text) override;
	virtual std::string getText() override;
	void keyPressEvent(QKeyEvent* event) override;


};
