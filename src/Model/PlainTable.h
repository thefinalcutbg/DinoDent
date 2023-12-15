#pragma once
#include <string>
#include <vector>

struct PlainCell {

	enum Icon { NoIcon, NHIF, HIS, PHIF, BDAY };

	std::string data;
	Icon icon{ Icon::NoIcon };
};

struct PlainColumn {
	
	enum Alignment { Left, Center, Right };
	
	std::string name;
	int width = 100;
	Alignment alignment{ Alignment::Left };

	std::vector<PlainCell> rows;
	
};

#include "Model/Financial/BusinessOperation.h"
#include "Model/Dental/Procedure.h"
#include "Model/Prescription/Medication.h"

struct PlainTable {

	std::vector<PlainColumn> data;

	PlainColumn& operator [] (int index) { return data[index]; };
	const PlainColumn& operator []  (int index) const { return data[index]; };

	int size() const { return data.size(); }

	int columnCount() const { return size(); }

	int rowCount() const {
		
		if(!size()) return 0;

		return data[0].rows.size();
	}

	void addColumn(const PlainColumn& p) { data.push_back(p); }
	void addCell(int column, const PlainCell& c) { data[column].rows.push_back(c); }

	PlainTable() {};

	PlainTable(const std::vector<Procedure>& pList);
	PlainTable(const std::vector<BusinessOperation>& bList);
	PlainTable(const std::vector<Medication>& mList);

};


//typedef std::vector<PlainColumn> PlainTable;