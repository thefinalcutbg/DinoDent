#pragma once

#include <QTextEdit>
#include <string>

class NotesEdit : public QTextEdit
{

	QString getCurrentDateStr();
	void keyPressEvent(QKeyEvent* event) override;

public:
	NotesEdit(QWidget* parent = nullptr);
	void setText(const std::string& text);
	std::string getText();

};