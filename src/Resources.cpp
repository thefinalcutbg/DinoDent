#include "Resources.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include "QtVersion.h"

std::string Resources::fromPath(const char* path)
{
	QFile file(path);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream in(&file);

#ifndef QT6
	in.setCodec("UTF-8");
#endif

	QString result = in.readAll();

	return result.toStdString();
}

std::string Resources::citiesJson() { return fromPath(":/json/json_ekatte.json");}
std::string Resources::nhifDataJson() { return fromPath(":/json/json_nhif.json"); }
std::string Resources::defaultPriceListJson() { return fromPath(":/json/json_defaultPriceList.json"); }
std::string Resources::defaultDiagnosisListJson() { return fromPath(":/json/json_diagnosis.json"); }
std::string Resources::ksmpJson() { return fromPath(":/json/json_ksmp.json"); }
std::string Resources::mkbJson() { return fromPath(":/json/json_mkb.json"); }

std::vector<std::string> Resources::dbSchema() {

	std::vector<std::string> result;

	QFile inputFile(":/db/dbSchema.txt");
	if (inputFile.open(QtIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{ 
			
			result.emplace_back(std::move(in.readLine().toStdString()));
		}
		inputFile.close();
	}
	return result;
}

std::vector<std::string> Resources::getMigrationScript(int version)
{
	std::vector<std::string> result;

	QString path = ":/migrations/db_migrateTo";
	path += QString::number(version);
	path += ".txt";

	QFile inputFile(path);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			result.emplace_back(std::move(in.readLine().toStdString()));
		}
		inputFile.close();
	}
	return result;
}

