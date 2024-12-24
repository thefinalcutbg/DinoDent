#pragma once

class NotificationListDialog;

class NotificationListPresenter
{
    NotificationListDialog* view;

public:
    NotificationListPresenter();
    void open();
};
