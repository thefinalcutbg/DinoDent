#include "NameFilterProxyModel.h"
#include <QDate>

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

DateSortProxyModel::DateSortProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void DateSortProxyModel::setDateColumn(int column)
{
    m_dateColumn = column;
    invalidate();
}

int DateSortProxyModel::dateColumn() const
{
    return m_dateColumn;
}

bool DateSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if (left.column() == m_dateColumn && right.column() == m_dateColumn) {
        const QString leftText =
            sourceModel()->data(left, sortRole()).toString().trimmed();
        const QString rightText =
            sourceModel()->data(right, sortRole()).toString().trimmed();

        const QDate leftDate = QDate::fromString(leftText, "dd.MM.yyyy");
        const QDate rightDate = QDate::fromString(rightText, "dd.MM.yyyy");

        const bool leftValid = leftDate.isValid();
        const bool rightValid = rightDate.isValid();

        if (leftValid && rightValid)
            return leftDate < rightDate;

        if (leftValid != rightValid)
            return leftValid; // valid dates first

        return QString::localeAwareCompare(leftText, rightText) < 0;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
