#include "NameFilterProxyModel.h"


bool NameFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	auto str = sourceModel()->index(sourceRow, filterKeyColumn()).data().toString().toLower();

	for (auto& name : m_names) {
		if (!str.contains(name)) return false;
	}

	return true;
}

void NameFilterProxyModel::setName(const QString& name)
{
	m_names = name.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

	for (auto& name : m_names) {
		name = name.toLower();
	}

	invalidateRowsFilter();
}
