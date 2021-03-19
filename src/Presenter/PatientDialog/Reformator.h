#pragma once
#include <QString>
#include <QDebug>

class Reformator
{
public:
	virtual QString reformat(QString text) = 0;
};

class NameReformator : public Reformator
{
public:
	QString reformat(QString text);
};

