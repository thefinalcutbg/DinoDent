#pragma once
#include <QSortFilterProxyModel>

class NameFilterProxyModel : public QSortFilterProxyModel
{
	QStringList m_names;
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public:
	void setName(const QString& name);
};

class DateSortProxyModel : public QSortFilterProxyModel
{
public:
    explicit DateSortProxyModel(QObject* parent = nullptr);

    void setDateColumn(int column);
    int dateColumn() const;

protected:
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    int m_dateColumn = -1;
};
