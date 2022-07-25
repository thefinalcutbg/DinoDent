#include "Resources.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
std::string cities;
std::string diagnosisList;
std::string priceList;
std::string nzokData;
std::string ksmp;

std::string getStringFromQtResource(const QString& path)
{
	QFile file(path);
	file.open(QIODeviceBase::ReadOnly);
	QTextStream in(&file);
	
	QString result = in.readAll();



	return result.toStdString();
}

void Resources::initialize()
{
	cities = getStringFromQtResource(":/json/json_cities.json");
	priceList = getStringFromQtResource(":/json/json_defaultPriceList.json");
	diagnosisList = getStringFromQtResource(":/json/json_diagnosis.json");
	nzokData = getStringFromQtResource(":/json/json_nzok.json");
	ksmp = getStringFromQtResource(":/json/json_ksmp.json");

}

const std::string& Resources::citiesJson() { return cities; }

const std::string& Resources::nzokDataJson() { return nzokData; }

const std::string& Resources::defaultPriceListJson() { return priceList; }

const std::string& Resources::defaultDiagnosisListJson() { return diagnosisList; }

const std::string& Resources::ksmpJson() { return ksmp; }

