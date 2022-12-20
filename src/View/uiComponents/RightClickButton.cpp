#include "RightClickButton.h"


RightClickButton::RightClickButton(QWidget* parent) :
    QPushButton(parent)
{

}

void RightClickButton::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::RightButton) {
        emit rightClicked();
        return;
    }
    
    QPushButton::mousePressEvent(e);
}