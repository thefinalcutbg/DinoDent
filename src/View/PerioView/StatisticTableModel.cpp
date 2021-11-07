 #include "StatisticTableModel.h"
#include "View/GlobalFunctions.h"

constexpr int numberOfRows{ 5 };
constexpr int numberOfColumns{ 6 };

static constexpr std::string_view IDX[numberOfRows]
{
	"FMPS",
	"FMBS", 
	"BOP", 
	"Костна загуба", 
	"Липсващи зъби(без мъдреци)"
};

static constexpr std::string_view PD[numberOfRows]
{
	u8"Средна дълбочина", 
	u8"до 3мм вкл.", 
	u8"4 и 5 мм.",
	u8"6 и 7 мм.", 
	u8"над 7 мм."
};

static constexpr std::string_view CAL[numberOfRows]
{
	u8"Средна загуба",
	u8"Разпространение", 
	u8"1 и 2 мм.",
	u8"3 и 4 мм.", 
	u8"5 мм. и повече"
};

enum Columns { IDXLabels, IDXData, PDLabels, PDData, CALLabels, CALData};

int StatisticTableModel::rowCount(const QModelIndex& parent) const
{
	return numberOfRows;
}

int StatisticTableModel::columnCount(const QModelIndex& parent) const
{
	return numberOfColumns;
}

StatisticTableModel::~StatisticTableModel()
{
}


QVariant StatisticTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal) {
			switch (section)
			{
			case 0: return "Индекси";
			case 2: return "Дълбочина на джобове";
			case 4: return "Загуба на аташман";
			}
		}
	}

	return QVariant();
}

QVariant StatisticTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) return QVariant();

	int row = index.row();
	int column = index.column();

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (column)
			{
			case IDXLabels: return IDX[row].data();
			case IDXData: return m_IDX[row];
			case PDLabels: return PD[row].data();
			case PDData: return m_PDData[row];
			case CALLabels: return CAL[row].data();
			case CALData: return m_CALData[row];

			}
		}
		case Qt::TextAlignmentRole: return int(Qt::AlignLeft | Qt::AlignVCenter);

	}
	return QVariant();
}

StatisticTableModel::StatisticTableModel(QObject* parent) : QAbstractTableModel(parent)
{
}

void StatisticTableModel::setStatistics(const PerioStatistic& stat)
{
	beginResetModel();
	
	m_IDX[0] = roundDouble(stat.HI) + " %";
	m_IDX[1] = roundDouble(stat.BI) + " %";
	m_IDX[2] = roundDouble(stat.BOP) + " %";
	m_IDX[3] = roundDouble(stat.boneIdx);
	m_IDX[4] = QString::number(stat.missingTeeth);


	m_PDData[0] = roundDouble(stat.pdAverage) + " mm";

	for(int i = 0, j = 1; i < stat.pdHistogramCount.size(); i++, j++){
		m_PDData[j] = 
					QString::number(stat.pdHistogramCount[i]) + " (" +
					roundDouble(stat.pdHistogramPercentage[i]) + "%)";
	}

	m_CALData[0] = roundDouble(stat.calAverage) + " mm";
	m_CALData[1] = roundDouble(stat.calDistribution) + " %";
	

	for (int i = 0, j = 2; i < stat.calHistogramCount.size(); i++, j++){
		m_CALData[j] = 
					QString::number(stat.calHistogramCount[i]) + " (" +
					roundDouble(stat.calHistogramPercentage[i]) + "%)";
	}

	endResetModel();
}

