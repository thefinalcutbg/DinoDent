#pragma once
#include "Model/Dental/ProcedureListElement.h"
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <vector>

struct ProcedureRow
{
	enum UserRole { None, Nhif, Fav, NonFav };

	QString code;
	QString displayCode;
	QString name;
	UserRole role{ UserRole::None };
	double price;
	QString displayPrice;
};

class ProcedureTemplateModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<ProcedureRow> procedures;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;


public:
	ProcedureTemplateModel(QObject *parent = nullptr);

	void setProcedures(std::vector<ProcedureListElement> procedures, bool addSections);
	const ProcedureRow& getProcedureRow(int index) const;
	~ProcedureTemplateModel();


	//void removeSelectedRow();
};

class QMouseEvent;

class FavButtonDelegate : public QItemDelegate
{
	Q_OBJECT

	QIcon nhif_pixmap{ ":/icons/icon_nhif.png" };
	QIcon star_yellow{ ":/icons/icon_fav.png" };
	QIcon star_gray{ ":/icons/icon_favgray.png" };
	QIcon star_hover{ ":/icons/icon_favhover.png" };

	int m_row_hover = -1;

	bool mouseIsOnStar(QMouseEvent* e, const QStyleOptionViewItem& option);

public:
	FavButtonDelegate(QObject* parent = 0);
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);

signals:
	void favouriteClicked(int row);
	void updateRequested();
};