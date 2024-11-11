#include "ProcedureTemplateModel.h"
#include <QIcon>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include "View/Theme.h"
#include "View/GlobalFunctions.h"

ProcedureTemplateModel::ProcedureTemplateModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void ProcedureTemplateModel::setProcedures(std::vector<ProcedureListElement> procedures)
{
    beginResetModel();


    this->procedures.clear();

    this->procedures.reserve(procedures.size());

    int block = -1;

    for (auto &m : procedures)
    {
        int currentBlock = m.nhif ? 0 : m.code.achiBlock();

        if (block != currentBlock) {

            QString code = m.nhif ? "НЗОК" : m.code.achiBlockName().c_str();

            code = code.toUpper();

            block = currentBlock;

            this->procedures.push_back(
                ProcedureRow{
                    .displayCode = m.nhif ? "" : QString::number(currentBlock),
                    .name = code,
                }
            );
        }


        ProcedureRow row{
            .code = m.code.code().c_str(),
            .displayCode = m.code.ACHICode().c_str(),
            .name = m.code.name().c_str(),
            .price = m.price,
            .displayPrice = m.price || m.nhif ? formatDoubleWithDecimal(m.price) : ""
        };

        if (row.code.isEmpty()) {
            row.role = ProcedureRow::None;
        }
        else if (m.nhif) {
            row.displayCode = QString::number(m.code.nhifCode());
            row.role = ProcedureRow::Nhif;
        }
        else {
            row.role = m.favourite ?
                ProcedureRow::Fav
                :
                ProcedureRow::NonFav;
        }

        this->procedures.push_back(row);

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
                case 1: return "НЗИС код";
                case 2: return "Код";
                case 3: return "Процедура";
                case 4: return "Цена";
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
    return 5;
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
    case  Qt::FontRole:
        //HEADER
        if (procedures[row].role == ProcedureRow::None) {
            QFont font;
            font.setBold(true);
            return font;
        }
        return QVariant();

    case Qt::UserRole:
        switch (column) 
        {
            case 0: return procedures[row].role;
            case 3: return procedures[row].price;
            default: return QVariant();

        }

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
/* 
//HEADER
    case Qt::BackgroundRole:
    {
     
   if (procedures[row].role == ProcedureRow::None) {
            return QVariant(QBrush(QColor(Theme::background)));
        }

        return QVariant();
    }
*/
    case Qt::DisplayRole:
        switch (column)
        {
            case 0: return index.row();
            case 1: return procedures[row].code;
            case 2: return procedures[row].displayCode;
            case 3: return procedures[row].name;
            case 4: return procedures[row].displayPrice;
            default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        if (column == 2 ||column == 4)
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
    w = 20;
    h = 20;
    x = r.left() + (r.width() - w) / 2;
    y = r.top() + (r.height() - h) / 2;

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
    w = 20;
    h = 20;
    x = r.left() + (r.width() - w) / 2;
    y = r.top() + (r.height() - h) / 2;

    auto starRect = QRect(x, y, w, h);

    const QIcon* px = nullptr;

    switch (role) {

        case  ProcedureRow::Nhif:
            px = &nhif_pixmap;
            break;
        case ProcedureRow::Fav:
            px = &star_yellow;
            break;
        case ProcedureRow::NonFav:
            px = &star_gray;
            break;
        case ProcedureRow::None:
            if (!option.state.testFlag(QStyle::State_Selected)) {
            //    painter->fillRect(r, Theme::background);
            }
            break;
    }

    if (!px) return;

    px->paint(painter, starRect);

    if(role == ProcedureRow::Fav || role == ProcedureRow::NonFav)

    if (m_row_hover == index.row() && m_row_hover != -1)
    {
        star_hover.paint(painter, starRect);
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