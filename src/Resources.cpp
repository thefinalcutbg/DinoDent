#include "Resources.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

std::string Resources::fromPath(const char* path)
{
	QFile file(path);
	file.open(QIODeviceBase::ReadOnly);
	QTextStream in(&file);

	QString result = in.readAll();

	return result.toStdString();
}

std::string Resources::citiesJson() { return fromPath(":/json/json_ekatte.json");}
std::string Resources::nzokDataJson() { return fromPath(":/json/json_nzok.json"); }
std::string Resources::defaultPriceListJson() { return fromPath(":/json/json_defaultPriceList.json"); }
std::string Resources::defaultDiagnosisListJson() { return fromPath(":/json/json_diagnosis.json"); }
std::string Resources::ksmpJson() { return fromPath(":/json/json_ksmp.json"); }
std::string Resources::mkbJson() { return fromPath(":/json/json_mkb.json"); }

std::vector<std::string> Resources::dbSchema() {

	std::vector<std::string> result;

	QFile inputFile(":/db/dbSchema.txt");
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{ 
			
			result.emplace_back(std::move(in.readLine().toStdString()));
			qDebug() << result.back().c_str();
		}
		inputFile.close();
	}
	return result;
}

