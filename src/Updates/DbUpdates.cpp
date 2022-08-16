#include "DbUpdates.h"
#include "Database/Database.h"

#include <unordered_map>
#include "Model/Date.h"
#include "Model/Parser/Parser.h"
#include "Model/Tooth/ToothContainer.h"
#include "Model/Financial/Invoice.h"
#include "Database/DbInvoice.h"
#include "QDebug"

void DbUpdates::reformatDates()
{
	/*
	{
		std::string query{
			"SELECT rowid, birth FROM PATIENT"
		};

		std::unordered_map<long long, Date> memory;

		for (Db db(query); db.hasRows();)
		{
			memory[db.asRowId(0)] = Date(db.asString(1));
		}

		for (auto &pair : memory)
		{
			std::string updateQuery{
				"UPDATE patient SET birth='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}
	}
	
	{
		std::string query{
			"SELECT rowid, status FROM amblist"
		};

		std::unordered_map<long long, std::string> memory;

		int rowidCount = 0;
	
		for (Db db(query); db.hasRows();)
		{	
			qDebug() << "parsing " << rowidCount; rowidCount++;

			ToothContainer t;

			Parser::parse(db.asString(1), t);

			memory[db.asRowId(0)] = Parser::write(t);
		}

		rowidCount = 0;

		for (auto& pair : memory)
		{
			qDebug() << "writing" << rowidCount; rowidCount++;

			std::string updateQuery{
				"UPDATE amblist SET status='" + pair.second + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}
	}
	
	{
		std::string query{
			"SELECT rowid, data FROM financial WHERE month_notif = 0"
		};

		std::unordered_map<long long, std::string> memory;

		int rowidCount = 0;

		for (Db db(query); db.hasRows();)
		{
			qDebug() << "parsing " << rowidCount; rowidCount++;

			Invoice inv;
			Parser::parse(db.asString(1), inv);

		
			memory[db.asRowId(0)] = Parser::write(inv);
		}

		rowidCount = 0;

		for (auto& pair : memory)
		{
			qDebug() << "writing" << rowidCount; rowidCount++;

			std::string updateQuery{
				"UPDATE financial SET data='" + pair.second + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}
	}
	


	{
		int rowidCount = 0;

		Db::crudQuery("ALTER TABLE ambList ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01Z00:00:00'");
		
		std::unordered_map<long long, Date> memory;

		std::string query{
			"SELECT rowid, day, month, year FROM ambList"
		};

		for (Db db(query); db.hasRows();)
		{
			qDebug() << "parsing " << rowidCount; rowidCount++;

			memory[db.asRowId(0)] = Date(db.asInt(1), db.asInt(2), db.asInt(3));
		}

		rowidCount = 0;

		for (auto& pair : memory)
		{
			qDebug() << "writing" << rowidCount; rowidCount++;

			std::string updateQuery{
				"UPDATE ambList SET date='" + pair.second.to8601() + "Z00:00:00" "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}

	}

	{
		int rowidCount = 0;

		Db::crudQuery("ALTER TABLE procedure ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01'");
		std::unordered_map<long long, Date> memory;

		std::string query{
			"SELECT procedure.rowid, procedure.day, amblist.month, amblist.year FROM procedure LEFT JOIN ambList ON procedure.amblist_rowid = amblist.rowid"
		};

		for (Db db(query); db.hasRows();)
		{
			qDebug() << "parsing " << rowidCount; rowidCount++;

			memory[db.asRowId(0)] = Date(db.asInt(1), db.asInt(2), db.asInt(3));
		}

		rowidCount = 0;

		for (auto& pair : memory)
		{
			qDebug() << "writing" << rowidCount; rowidCount++;

			std::string updateQuery{
				"UPDATE procedure SET date='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}

	}
	
	{
		int rowidCount = 0;

		Db::crudQuery("ALTER TABLE financial ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01'");
		std::unordered_map<long long, Date> memory;

		std::string query{
			"SELECT rowid, day, month, year FROM financial"
		};

		for (Db db(query); db.hasRows();)
		{
			qDebug() << "parsing " << rowidCount; rowidCount++;

			memory[db.asRowId(0)] = Date(db.asInt(1), db.asInt(2), db.asInt(3));
		}

		rowidCount = 0;

		for (auto& pair : memory)
		{
			qDebug() << "writing" << rowidCount; rowidCount++;

			std::string updateQuery{
				"UPDATE financial SET date='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}
	}

	{
		int rowidCount = 0;

		Db::crudQuery("ALTER TABLE periostatus ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01'");
		std::unordered_map<long long, Date> memory;

		std::string query{
			"SELECT rowid, day, month, year FROM financial"
		};

		for (Db db(query); db.hasRows();)
		{
			qDebug() << "parsing " << rowidCount; rowidCount++;

			memory[db.asRowId(0)] = Date(db.asInt(1), db.asInt(2), db.asInt(3));
		}

		rowidCount = 0;

		for (auto& pair : memory)
		{
			qDebug() << "writing" << rowidCount; rowidCount++;

			std::string updateQuery{
				"UPDATE periostatus SET date='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}

	}


		{
		int rowidCount = 0;

		std::unordered_map<std::string, std::optional<NzokContract>> memory;

		std::string query{
			"SELECT rzi, nzok_contract FROM practice"
		};

		for (Db db(query); db.hasRows();)
		{
			qDebug() << "parsing " << rowidCount; rowidCount++;

			memory[db.asString(0)] = Parser::parseContract(db.asString(1));
		}

		rowidCount = 0;

		for (auto& pair : memory)
		{
			qDebug() << "writing" << rowidCount; rowidCount++;

			std::string updateQuery{
				"UPDATE practice SET nzok_contract='" + Parser::write(pair.second) + "' "
				"WHERE rzi='" + pair.first + "'"
			};

			Db::crudQuery(updateQuery);
		}

	}

		*/
}




