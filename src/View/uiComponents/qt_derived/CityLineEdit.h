#pragma once
#include "LineEdit.h"
#include <QCompleter>
#include <QStringListModel>
#include <QTextStream>
#include <QFile>
#include <QAbstractItemView>

class CityLineEdit : public LineEdit
{

	QCompleter* cityCompleter;
	QStringList citySuggestions;

	int maxCharLength;

	void cityLoader();
	void setCityCompleter();
	void reformat() override;

public:

	CityLineEdit(QWidget* parent = 0);

};


