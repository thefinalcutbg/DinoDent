#include "PlainTable.h"
#include "FreeFunctions.h"
#include "View/GlobalFunctions.h"

PlainTable::PlainTable(const std::vector<Procedure>& pList)
{
    addColumn({ .name = "Дата",.width = 100,.alignment = PlainColumn::Right });
    addColumn({ .name = "Диагноза", .width = 200 });
    addColumn({ .name = "Зъб", .width = 70, .alignment = PlainColumn::Center });
    addColumn({ .name = "Манипулация", .width = 300 });
    addColumn({ .name = "Код", .width = 50, .alignment = PlainColumn::Center });

    for (auto& p : pList) {

        addCell(0, {
            .data = p.date.toBgStandard(),
            .icon = p.isSentToHis() ? PlainCell::HIS : PlainCell::NOICON
            });

        addCell(1, { p.diagnosis.getFullDiagnosis() });
        addCell(2, { p.getToothString() });
        addCell(3, { p.code.name() });

        PlainCell::Icon icon;

        switch (p.financingSource) {
        case FinancingSource::NHIF: icon = PlainCell::NHIF; break;
        case FinancingSource::PHIF: icon = PlainCell::PHIF; break;
        default: icon = PlainCell::NOICON;
        }

        addCell(4, PlainCell{
            .data = std::to_string(p.code.oldCode()),
            .icon = icon
            });
    }
}

PlainTable::PlainTable(const std::vector<BusinessOperation>& bList)
{
    addColumn({ .name = "Код",.width = 100,.alignment = PlainColumn::Center });
    addColumn({ .name = "Наименование", .width = 400 });
    addColumn({ .name = "Кол-во", .width = 70, .alignment = PlainColumn::Center });
    addColumn({ .name = "Ед. цена", .width = 70, .alignment = PlainColumn::Center });
    addColumn({ .name = "Сума", .width = 70, .alignment = PlainColumn::Center });

    for (auto& op : bList)
    {
        addCell(0, { op.activity_code });
        addCell(1, { op.activity_name });
        addCell(2, { std::to_string(op.quantity) });
        addCell(3, { priceToString(op.unit_price).toStdString() });
        addCell(4, { priceToString(op.value_price).toStdString() });
    }

}

PlainTable::PlainTable(const std::vector<Medication>& mList)
{
    addColumn({ .name = "Медикамент", .width = 350 });
    addColumn({ .name = "Дозировка" });

    for (auto& m : mList)
    {
        addCell(0, { m.name() });

        std::string dosages;

        for (auto& d : m.dosageList())
        {
            dosages.append(d);
            dosages += "; ";
        }
        addCell(1, { dosages });

    }
}