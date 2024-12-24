#include "NotificationTableModel.h"

#include <QEvent>
#include <QMouseEvent>

NotificationTableModel::NotificationTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{}

QVariant NotificationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return "";
            case 1: return "Дата";
            case 2: return "Име на пациент";
            case 3: return "Телефон";
            case 4: return "Описание";
            case 5: return "";
            }
        }
    }

    if(role == Qt::TextAlignmentRole){
        return int(Qt::AlignVCenter | Qt::AlignHCenter);
    }

    return QVariant();
}

void NotificationTableModel::setNotifications(const std::vector<Notification> &nList, bool future)
{
    Date currentDate = Date::currentDate();

    beginResetModel();

    m_notif.clear();

    for(auto& n : nList)
    {
        bool isFutureNotif = n.date > currentDate;

        if(isFutureNotif == future){
            m_notif.push_back(n);
        }
    }

    endResetModel();
}

QVariant NotificationTableModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    if (row == m_notif.size()) return 0;
    if (m_notif.size() == 0) return 0;

    switch (role)
    {
    case Qt::DisplayRole:
        switch (column)
        {
        case 0: return m_notif[row].patientRowid;
        case 1: return m_notif[row].date.toBgStandard().c_str();
        case 2: return m_notif[row].patientName.c_str();
        case 3: return m_notif[row].phone.c_str();
        case 4: return m_notif[row].description.c_str();
        case 5: return m_notif[row].rowid;
        default: return QVariant();
        }

    case Qt::TextAlignmentRole:
        if(column > 1)
            return int(Qt::AlignVCenter | Qt::AlignLeft);
        else
            return int(Qt::AlignVCenter | Qt::AlignHCenter);
    }

    return QVariant();
}



bool NotificationRemoveDelegate::mouseIsOnIcon(QMouseEvent *e, const QStyleOptionViewItem &option)
{
    int clickX = e->position().x();
    int clickY = e->position().y();

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

void NotificationRemoveDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect r = option.rect;//getting the rect of the cell
    int x, y, w, h;
    w = 20;
    h = 20;
    x = r.left() + (r.width() - w) / 2;
    y = r.top() + (r.height() - h) / 2;

    auto iconRect = QRect(x, y, w, h);

    QIcon::Mode iconMode = m_row_hover == index.row() ? QIcon::Mode::Active : QIcon::Mode::Disabled;

    remove_icon.paint(painter, iconRect, Qt::AlignCenter, iconMode);
}

bool NotificationRemoveDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (!index.data().toLongLong()) {
        return false;
    }

    if (event->type() == QEvent::MouseMove) {

        m_row_hover = mouseIsOnIcon((QMouseEvent*)event, option) ? index.row() : -1;

        emit updateRequested();

        return true;
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        if (!mouseIsOnIcon((QMouseEvent*)event, option)) return false;

        emit removeClicked(index.data().toLongLong());

        return false;

    }

    return false;
}
