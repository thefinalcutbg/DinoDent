#include "DbUpdates.h"
#include "Database/Database.h"
#include "JsonCpp/json.h"
#include "Model/Dental/NhifSheetData.h"
#include <QFile>
#include "View/Widgets/UpdateDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Path.h"
#include "Model/FreeFunctions.h"

void DbUpdates::backupDatabase()
{
	auto time = Time::currentTime();

	auto backupPath = Path::getDataPath() + 
					  "/" + 
					  "backup" +
					  Date::currentDate().to8601() + "T" +
					  FreeFn::leadZeroes(time.hour, 2) + "-" +
					  FreeFn::leadZeroes(time.minutes, 2) + "-" +
					  FreeFn::leadZeroes(time.sec, 2) +
					  ".db"
	;

	QFile::copy(Db::getFilePath().c_str(), backupPath.c_str());

}
