#include "CityLineEdit.h"

CityLineEdit::CityLineEdit(QWidget* parent) : LineEdit(parent), maxCharLength(70)
{
	cityLoader();
	setCityCompleter();

	connect(this, &QLineEdit::textChanged, [=] { AbstractUIElement::validateInput(); }); //because it's also changed programatically
}

#include "Model/CityCode.h"

void CityLineEdit::cityLoader()
{
	CityCode cityCode;

	for (auto& it : cityCode.getMap())
	{
		QString cityString = QString::fromStdString(it.first);
		citySuggestions.append(cityString);

		if (cityString.length() > maxCharLength)
			maxCharLength = cityString.length();
	}

	setMaxLength(maxCharLength);

	citySuggestions.sort();

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
