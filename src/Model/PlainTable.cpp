﻿#include "PlainTable.h"
#include "FreeFunctions.h"

void PlainTable::addEmptyRow()
{
    for (auto& column : data) {
        column.rows.push_back(PlainCell{""});
    }
}

PlainTable::PlainTable(const std::vector<Procedure>& pList)
{
    addColumn({"Дата",100,PlainColumn::Right });
    addColumn({"Диагноза", 200 });
    addColumn({"Зъб", 70, PlainColumn::Center });
    addColumn({"Процедура", 300 });
    addColumn({"Код", 50, PlainColumn::Center });

    for (auto& p : pList) {

        addCell(0, {
            .data = p.date.toBgStandard(),
            .icon = p.isSentToHis() ? CommonIcon::HIS : CommonIcon::NOICON
            });

        addCell(1, { p.diagnosis.getDiagnosisText() });
        addCell(2, { p.getToothString() });
        addCell(3, { p.code.name() });

        CommonIcon::Type icon;

        switch (p.financingSource) {
        case FinancingSource::NHIF: icon = CommonIcon::NHIF; break;
        case FinancingSource::PHIF: icon = CommonIcon::PHIF; break;
        case FinancingSource::Patient: icon = CommonIcon::PATIENT; break;
        default: icon = CommonIcon::NOICON;
        }

        addCell(4, PlainCell{
            .data = p.code.ACHICode().c_str(),
            .icon = icon
            });
    }
}

PlainTable::PlainTable(const std::vector<BusinessOperation>& bList)
{
    addColumn({"Код",100,PlainColumn::Center });
    addColumn({"Наименование", 400 });
    addColumn({"Кол-во", 70, PlainColumn::Center });
    addColumn({"Ед. цена", 70, PlainColumn::Center });
    addColumn({"Сума", 70, PlainColumn::Center });

    for (auto& op : bList)
    {
        addCell(0, { op.activity_code });
        addCell(1, { op.activity_name });
        addCell(2, { std::to_string(op.quantity) });
        addCell(3, { FreeFn::formatDouble(op.unit_price) });
        addCell(4, { FreeFn::formatDouble(op.value_price) });
    }

}

PlainTable::PlainTable(const std::vector<Medication>& mList)
{
    addColumn({"Медикамент", 350 });
    addColumn({"Инструкции за прием"});

    for (auto& m : mList)
    {
        addCell(0, { m.name() });

        addCell(1, { m.parseFullDosage() });

    }
}
