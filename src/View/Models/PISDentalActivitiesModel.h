#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "Model/Dental/Procedure.h"

class PisDentalActivitiesModel : public QAbstractTableModel
{
	Q_OBJECT

	struct DentalActivities {
		QString date;
		QString diagnosis;
		int tooth;
		QString name;
		QString code;
		QString payment;
	};

	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	std::vector<DentalActivities> m_procedures;

	static constexpr int poceduresColumnCount = 6;

public:


	PisDentalActivitiesModel(QObject* parent = nullptr);
	void setProcedures(const std::vector<Procedure>& p, const std::vector<std::string>& payment_stat);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	~PisDentalActivitiesModel();

	//void removeSelectedRow();
};
