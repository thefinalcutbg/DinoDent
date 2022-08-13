#include "Resources.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

inline  const std::string fromQtResource(const QString& path)
{
	QFile file(path);
	file.open(QIODeviceBase::ReadOnly);
	QTextStream in(&file);

	QString result = in.readAll();

	return result.toStdString();
}

const std::string Resources::citiesJson() { return fromQtResource(":/json/json_ekatte.json");}
const std::string Resources::nzokDataJson() { return fromQtResource(":/json/json_nzok.json"); }
const std::string Resources::defaultPriceListJson() { return fromQtResource(":/json/json_defaultPriceList.json"); }
const std::string Resources::defaultDiagnosisListJson() { return fromQtResource(":/json/json_diagnosis.json"); }
const std::string Resources::ksmpJson() { return fromQtResource(":/json/json_ksmp.json"); }
const std::string Resources::mkbJson(){ return fromQtResource(":/json/json_mkb.json"); }

