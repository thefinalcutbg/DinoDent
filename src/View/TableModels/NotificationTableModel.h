#pragma once

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QIcon>
#include "Model/Dental/Notification.h"

class NotificationTableModel : public QAbstractTableModel
{
    std::vector<Notification> m_notif;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override { return m_notif.size(); }
    int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 6; };
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;


public:
    explicit NotificationTableModel(QObject *parent = nullptr);
    void setNotifications(const std::vector<Notification>& nList, bool future = false);
};

class QMouseEvent;

class NotificationRemoveDelegate : public QItemDelegate
{
    Q_OBJECT

    QIcon remove_icon{ ":/icons/icon_remove.png" };

    int m_row_hover = -1;

    bool mouseIsOnIcon(QMouseEvent* e, const QStyleOptionViewItem& option);

public:
    NotificationRemoveDelegate(QObject* parent = 0) : QItemDelegate(parent) {};
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);

signals:
    void removeClicked(long long rowId);
    void updateRequested();
};
