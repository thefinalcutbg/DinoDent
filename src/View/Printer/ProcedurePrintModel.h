#pragma once
#include <qabstractitemmodel.h>
#include <vector>
#include "Model/Procedure/Procedure.h"

struct PrintProcedure
{
    QString date, diagnosis, tooth, proc, code, dur;
};

class ProcedurePrintModel : public QAbstractTableModel
{
    std::vector<PrintProcedure> m_procedures;

public:
    
    ProcedurePrintModel(const std::vector<Procedure>& procedures = std::vector<Procedure>{},
        const std::array<bool, 32>& tempTeeth = std::array<bool, 32>{false}
    );

    int rowCount(const QModelIndex& parent) const  override { return m_procedures.size(); }
    int columnCount(const QModelIndex& parent) const  override { return 6; }
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

};