#include "CityLineEdit.h"

CityLineEdit::CityLineEdit(QWidget* parent) : LineEdit(parent), maxCharLength(70)
{
	cityLoader();
	setCityCompleter();

	connect(this, &QLineEdit::textChanged, [=] { AbstractUIElement::validateInput(); }); //because it's also changed programatically
}

void CityLineEdit::cityLoader()
{
	QFile* file = new QFile("cities.txt");
	if (!file->open(QFile::ReadOnly | QFile::Text)) {}
	QTextStream in(file);
	in.setCodec("UTF-8");

	int maxCharLength = 0;

	while (!in.atEnd())
	{
		QString line = in.readLine();
		citySuggestions.append(line);

		if (line.length() > maxCharLength)
			maxCharLength = line.length();
	}

	setMaxLength(maxCharLength);

	citySuggestions.sort();

	file->close();
	delete file;
}

void CityLineEdit::setCityCompleter()
{
	cityCompleter = new QCompleter(citySuggestions, this);
	cityCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	cityCompleter->setCompletionMode(QCompleter::PopupCompletion);

	QFont f("Segoe UI");
	f.setPixelSize(10);
	cityCompleter->popup()->setFont(f);
	cityCompleter->popup()->setMinimumWidth(271);
	cityCompleter->setMaxVisibleItems(10);
	cityCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	this->setCompleter(cityCompleter);
}

void CityLineEdit::reformat()
{
	if (cityCompleter->popup()->isVisible())
		QLineEdit::setText(cityCompleter->currentCompletion());
	validateInput();
}
