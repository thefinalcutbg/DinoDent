#pragma once

#include <QEvent>
#include <QWidget>

class MouseWheelGuard : public QObject
{
public:
    explicit MouseWheelGuard(QWidget* parent) : QObject(parent)
    {
        parent->setFocusPolicy(Qt::StrongFocus);
    }

protected:
    bool eventFilter(QObject* o, QEvent* e) override
    {
        const QWidget* widget = static_cast<QWidget*>(o);
        if (e->type() == QEvent::Wheel && widget && !widget->hasFocus())
        {
            e->ignore();
            return true;
        }

        return QObject::eventFilter(o, e);
    }
};
