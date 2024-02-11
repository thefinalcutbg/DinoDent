#include "ProcedureTemplateModel.h"
#include <QIcon>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <qdebug.h>

ProcedureTemplateModel::ProcedureTemplateModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}



void ProcedureTemplateModel::setProcedures(std::vector<ProcedureListElement> procedures)
{
    beginResetModel();


    this->procedures.clear();

    this->procedures.reserve(procedures.size());

    for (auto &m : procedures)
    {
        this->procedures.emplace_back
        (
            ProcedureRow{ 
            .code = QString::number(m.code.oldCode()),
            .name = m.code.name().c_str(),
            .nhif = m.nhif, 
            .role = m.nhif ? ProcedureRow::None 
                    :
                    m.favourite ? ProcedureRow ::Fav
                    :
                    ProcedureRow::NonFav
            }
        );
    }

    endResetModel();
}

const ProcedureRow& ProcedureTemplateModel::getProcedureRow(int index) const
{
    return procedures[index];
}

ProcedureTemplateModel::~ProcedureTemplateModel()
{
}

QVariant ProcedureTemplateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0: return "";
                case 1: return "Код";
                case 2: return "Манипулация";
            }
        }
    }

    return QVariant();
}

int ProcedureTemplateModel::rowCount(const QModelIndex&) const
{
    return procedures.size();
}

int ProcedureTemplateModel::columnCount(const QModelIndex&) const
{
    return 3;
}


QVariant ProcedureTemplateModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == procedures.size()) return 0;
    if (procedures.size() == 0) return 0;

    switch (role)
    {
    case Qt::UserRole:
        return column == 0 ? procedures[row].role : QVariant();

/*
    case Qt::DecorationRole:
        switch (column)
        {
        case 1:
            if (procedures[row].nhif)
                return QIcon(":/icons/icon_nhif.png");
            break;
        default:
            return QVariant();
        }
*/

    case Qt::DisplayRole:
        switch (column)
        {
            case 0: return index.row();
            case 1: return procedures[row].code;
            case 2: return procedures[row].name;
            default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 1 ||column == 3)
            return int(Qt::AlignCenter);
    }

    return QVariant();
}



bool FavButtonDelegate::mouseIsOnStar(QMouseEvent* e, const QStyleOptionViewItem& option)
{
    int clickX = e->x();
    int clickY = e->y();

    QRect r = option.rect;//getting the rect of the cell
    int x, y, w, h;
    x = r.left()+5;//the X coordinate
    y = r.top()+5;//the Y coordinate
    w = 20;//button width
    h = 20;//button height

    if (clickX > x && clickX < x + w)
        if (clickY > y && clickY < y + h)
        {
            return true;
        }

    return false;
}

FavButtonDelegate::FavButtonDelegate(QObject* parent)
    : QItemDelegate(parent)
{

}

void FavButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto role = index.data(Qt::UserRole).toInt();

    QRect r = option.rect;//getting the rect of the cell
    int x, y, w, h;
    x = r.left()+5;//the X coordinate
    y = r.top()+5;//the Y coordinate
    w = 20;//button width
    h = 20;//button height
    auto starRect = QRect(x, y, w, h);

    const QPixmap* px = nullptr;

    if (role == ProcedureRow::None) {
        px = &nhif_pixmap;
    }
    else if (role == ProcedureRow::Fav) {
        px = &star_yellow;
    }
    else {
        px = &star_gray;
    }

    if (!px) return;

    painter->drawPixmap(starRect, px->scaled(20, 20, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));

    if (m_row_hover == index.row() && m_row_hover != -1)
    {
        painter->drawPixmap(starRect, star_hover.scaled(20, 20, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
    }
}


bool FavButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    
    if (!index.data(Qt::UserRole).toInt()) {
        return false;
    }

    if (event->type() == QEvent::MouseMove) {

        m_row_hover = mouseIsOnStar((QMouseEvent*)event, option) ? index.row() : -1;
       
        emit updateRequested();

        return true;
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        if (!mouseIsOnStar((QMouseEvent*)event, option)) return false;

        emit favouriteClicked(index.data().toInt());

        return false;

    }

    return false;
}