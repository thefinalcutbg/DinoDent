#pragma once

#include <QTextEdit>

#include "View/Interfaces/AbstractLineEdit.h"

class QLabel;

class TextEdit : public QTextEdit, public AbstractLineEdit
{
	Q_OBJECT

	QLabel* errorLabel{ nullptr };
	bool disabled{ false };
	void keyPressEvent(QKeyEvent* event) override;

public:
	TextEdit(QWidget *parent);
	~TextEdit();

	void setErrorLabel(QLabel* errorLabel);
	// Inherited via AbstractLineEdit
	virtual void setFocus() override;
	virtual void disable(bool disable) override;
	virtual void setValidAppearence(bool valid) override;
	virtual void set_Text(const std::string& text) override;
	virtual std::string getText() override;



};
