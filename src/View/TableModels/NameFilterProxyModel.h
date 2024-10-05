#pragma once
#include <QSortFilterProxyModel>

class NameFilterProxyModel : public QSortFilterProxyModel
{
	QStringList m_names;
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public:
	void setName(const QString& name);
};