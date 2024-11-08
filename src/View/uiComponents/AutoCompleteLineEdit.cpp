#include "AutoCompleteLineEdit.h"

AutoCompleteLineEdit::AutoCompleteLineEdit(QWidget* parent) : LineEdit(parent), maxCharLength(70)
{
    connect(this, &QLineEdit::textChanged, this, [&] {

        AbstractUIElement::validateInput();

        if(completer()) completer()->popup()->setMinimumWidth(width());

	}); //because it's also changed programatically
}

void AutoCompleteLineEdit::setCompletions(const std::vector<std::string>& vec)
{
	QStringList completions;

	for (auto& it : vec)
	{
		QString cityString = QString::fromStdString(it);
		completions.append(cityString);

		if (cityString.length() > maxCharLength)
			maxCharLength = cityString.length();
	}

	setMaxLength(maxCharLength);

	setCompletions(completions);
}


void AutoCompleteLineEdit::setCompletions(const std::unordered_map<std::string, int>& map)
{
	QStringList completions;

	for (auto& it : map)
	{
		QString cityString = QString::fromStdString(it.first);
		completions.append(cityString);

		if (cityString.length() > maxCharLength)
			maxCharLength = cityString.length();
	}

	setMaxLength(maxCharLength);
	
	completions.sort();

	setCompletions(completions);

}

void AutoCompleteLineEdit::reformat()
{
	if (!completer()) return;

	if (completer()->popup()->isVisible())
	{
		QLineEdit::setText(completer()->currentCompletion());
	}


	validateInput();
}

void AutoCompleteLineEdit::setCompletions(const QStringList& list)
{
	auto new_completer = new QCompleter(list, this);
	new_completer->setCaseSensitivity(Qt::CaseInsensitive);
	new_completer->setCompletionMode(QCompleter::PopupCompletion);


    QFont f;
    //f.setPixelSize(10);
	new_completer->popup()->setFont(f);
	new_completer->setMaxVisibleItems(10);
	new_completer->setModelSorting(QCompleter::UnsortedModel);
	this->setCompleter(new_completer);
}
