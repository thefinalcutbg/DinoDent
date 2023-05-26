#pragma once
#include <QPushButton>

class ShortcutTile : public QPushButton
{

    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

protected:

    bool hover{ false };
    bool clicked{ false };


public:
    ShortcutTile(QWidget* parent = 0);

};