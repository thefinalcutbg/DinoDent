#include "MKB.h"

#include <unordered_map>

#include "Database/Database.h"

#include "Resources.h"

std::unordered_map<std::string, std::string> mkbToName;
const std::string empty{};

void MKB::initialize() {

	mkbToName.clear();

	Db db("SELECT code, description FROM mkb");

	while (db.hasRows()) mkbToName[db.asString(0)] = db.asString(1);

}


const std::string& MKB::getNameFromMKBCode(const std::string& mkbCode)
{
	if (mkbToName.count(mkbCode)) return mkbToName.at(mkbCode);

	return empty;
}

bool MKB::isInitialized()
{
	return !mkbToName.empty();
}
