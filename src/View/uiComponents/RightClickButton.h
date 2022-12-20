#pragma once

#include <QPushButton>
#include <QMouseEvent>

class RightClickButton : public QPushButton
{
    Q_OBJECT

    void mousePressEvent(QMouseEvent* e) override;

public:
    explicit RightClickButton(QWidget* parent = 0);




signals:
    void rightClicked();

public slots:

};