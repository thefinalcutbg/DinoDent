#pragma once

#include <QPlainTextEdit>
#include <string>
#include <vector>

#include "View/Interfaces/AbstractLineEdit.h"

class QLabel;
class QCompleter;
class QString;
class QKeyEvent;

class TextEdit : public QPlainTextEdit, public AbstractLineEdit
{
	Q_OBJECT

	QLabel* errorLabel{ nullptr };
	bool disabled{ false };
	QCompleter* completer{ nullptr };

	void keyPressEvent(QKeyEvent* event) override;

public:
	explicit TextEdit(QWidget* parent);
	~TextEdit();

	void setErrorLabel(QLabel* errorLabel);
	void setCompletions(const std::vector<std::string>& completions);

	// Inherited via AbstractLineEdit
	virtual void set_focus() override;
	virtual void disable(bool disable) override;
	virtual void setValidAppearence(bool valid) override;
	virtual void set_Text(const std::string& text) override;
	virtual std::string getText() override;

private slots:
	void insertCompletion(const QString& completion);

private:
	QString textUnderCursor() const;
};