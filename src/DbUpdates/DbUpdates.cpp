#include "DbUpdates.h"
#include "Database/Database.h"
#include <QFile>
#include "GlobalSettings.h"

void DbUpdates::backupDatabase()
{
	QFile::copy(GlobalSettings::getDbSettings().sqliteFilePath.c_str(), GlobalSettings::getDbBackupFilepath().c_str());
}
