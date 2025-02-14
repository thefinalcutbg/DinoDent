#include "ColorPicker.h"
#include <QMenu>
#include <QPainter>
#include <set>
#include <vector>

template<>
struct std::hash<QColor>
{
    std::size_t operator()(const QColor& c) const noexcept
    {
        return std::hash<unsigned int>{}(c.rgba());
    }
};

const std::vector<QColor>& getAvailableColors() {

    static std::vector<QColor> colors{
        QColor(),
        QColorConstants::Svg::lightseagreen,
        QColorConstants::Svg::dodgerblue,
        QColorConstants::Svg::mediumpurple,
        QColorConstants::Svg::crimson,
        QColorConstants::Svg::hotpink,
        QColorConstants::Svg::darkorange,
        QColorConstants::Svg::goldenrod,
        QColorConstants::Svg::saddlebrown,
        QColorConstants::Svg::lightslategray
    };
    
    return colors;
}

ColorPicker::ColorPicker(QWidget* widget) :QToolButton(widget)
{
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    setColor(selected_color);

    setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu* contextMenu = new QMenu(this);


    for (auto& color : getAvailableColors()) {

        QAction* a = new QAction();

        a->setIcon(createIcon(color , QRect{ 0,0,22,22 }));

        a->setIconVisibleInMenu(true);

        connect(a, &QAction::triggered, this, [=] { setColor(color); });

        contextMenu->addAction(a);
    }

    setMenu(contextMenu);

    contextMenu->setMaximumWidth(30);

    setArrowType(Qt::ArrowType::NoArrow);
}

QIcon ColorPicker::createIcon(const QColor& c, const QRect& rect)
{
    QPixmap pixmap(rect.width(), rect.height());
    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    if (c.isValid()) {

        QColor color = c;
        color.setAlpha(255);


        painter.setPen(color);
        painter.setBrush(color);
        painter.drawRect(rect);
    }
    else {

        painter.fillRect(rect, Qt::white);

        painter.setPen(Qt::red);

        painter.drawLine(rect.bottomRight(), rect.topLeft());

        painter.setPen(Qt::black);

        painter.drawRect(rect);
    }

    QIcon icon;
    icon.addPixmap(pixmap);

    return icon;
}

QColor ColorPicker::color() const
{
    return selected_color;
}

void ColorPicker::setColor(const QColor& color_)
{
    selected_color = color_;

    setIcon(createIcon(color_, QRect{ 0,0,22,22 }));
}