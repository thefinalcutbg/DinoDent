#pragma once

#include <QTextEdit>
#include <QKeyEvent>
#include "../AbstractLineEdit.h"

class TextEdit : public QTextEdit, public AbstractLineEdit
{
	Q_OBJECT

	bool disabled{ false };
	void keyPressEvent(QKeyEvent* event) override;

public:
	TextEdit(QWidget *parent);
	~TextEdit();

	// Inherited via AbstractLineEdit
	virtual void setFocus() override;
	virtual void disable(bool disable) override;
	virtual void setValidAppearence(bool valid) override;
	virtual void set_Text(const std::string& text) override;
	virtual std::string getText() override;



};
