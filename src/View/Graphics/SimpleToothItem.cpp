#include "SimpleToothItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextLayout>
#include <QFontMetrics>
#include "View/Theme.h"

namespace
{
    static constexpr qreal kCellW = 43.0;
    static constexpr qreal kCellH = 70.0;
    static constexpr qreal kNumBoxH = kCellH - kCellW;

    static constexpr qreal kNumPadX = 2.0;
    static constexpr qreal kStatusPadX = 3.0;
    static constexpr qreal kStatusPadY = 2.0;

    static constexpr qreal kThinW = 2.0;
    static constexpr qreal kThickW = 4;

    static bool needsCircle(const QString& num)
    {
        bool ok = false;
        const int n = num.trimmed().toInt(&ok);
        return ok && (n > 48); // your rule: circle if number > 48
    }
}

SimpleToothItem::SimpleToothItem(int idx)
    : SelectableGraphicsItem(idx)
{
}

SimpleToothItem::~SimpleToothItem() = default;

void SimpleToothItem::setData(const std::string& num, const std::set<std::string>& statuses)
{
    m_num = QString::fromStdString(num);

    m_stat_list.clear();
    m_stat_list.reserve(static_cast<int>(statuses.size()));
    for (const auto& s : statuses)
        m_stat_list << QString::fromStdString(s);

    update();
}

QRectF SimpleToothItem::boundingRect() const
{
    return QRectF(0.0, 0.0, kCellW, kCellH);
}

static QPainterPath makeOuterPath(const QRectF& r, qreal rad, bool tl, bool tr, bool br, bool bl)
{
    const qreal x = r.x();
    const qreal y = r.y();
    const qreal w = r.width();
    const qreal h = r.height();
    const qreal rr = std::min(rad, std::min(w, h) * 0.5);

    QPainterPath p;

    p.moveTo(x + (tl ? rr : 0.0), y);

    p.lineTo(x + w - (tr ? rr : 0.0), y);
    if (tr) p.quadTo(x + w, y, x + w, y + rr);
    else    p.lineTo(x + w, y);

    p.lineTo(x + w, y + h - (br ? rr : 0.0));
    if (br) p.quadTo(x + w, y + h, x + w - rr, y + h);
    else    p.lineTo(x + w, y + h);

    p.lineTo(x + (bl ? rr : 0.0), y + h);
    if (bl) p.quadTo(x, y + h, x, y + h - rr);
    else    p.lineTo(x, y + h);

    p.lineTo(x, y + (tl ? rr : 0.0));
    if (tl) p.quadTo(x, y, x + rr, y);
    else    p.lineTo(x, y);

    p.closeSubpath();
    return p;
}

static qreal wrappedTextHeight(const QString& text, const QFont& font, qreal width)
{
    QTextLayout layout(text, font);
    layout.beginLayout();
    qreal h = 0.0;

    while (true)
    {
        QTextLine line = layout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(width);
        line.setPosition(QPointF(0.0, h));
        h += line.height();
    }

    layout.endLayout();
    return h;
}

static QFont fitFontToRectWrapped(const QString& text,
    QFont base,
    const QRectF& rect,
    int minPointSize)
{
    if (text.isEmpty())
        return base;

    int pt = base.pointSize();
    if (pt <= 0)
        pt = 10;

    for (int cur = pt; cur >= minPointSize; --cur)
    {
        base.setPointSize(cur);
        const qreal h = wrappedTextHeight(text, base, rect.width());
        if (h <= rect.height())
            return base;
    }

    base.setPointSize(minPointSize);
    return base;
}

void SimpleToothItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, false);

    bool is_selected = option && (option->state & QStyle::State_Selected);

    // drawing boxes
    QPen pen = Theme::fontTurquoiseClicked;
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    bool upper = (m_idx >= 0 && m_idx < 16);

    QRectF numRect = upper
        ? QRectF(0.0, 0.0, kCellW, kNumBoxH)
        : QRectF(0.0, kCellH - kNumBoxH, kCellW, kNumBoxH);

    QRectF statusRect = upper
        ? QRectF(0.0, kNumBoxH, kCellW, kCellH - kNumBoxH)
        : QRectF(0.0, 0.0, kCellW, kCellH - kNumBoxH);

    bool tl = (m_idx == 0);
    bool tr = (m_idx == 15);
    bool br = (m_idx == 16);
    bool bl = (m_idx == 31);

    if (tl || tr || br || bl)
    {
        auto path = makeOuterPath(QRectF(0.0, 0.0, kCellW, kCellH), 6.0, tl, tr, br, bl);

        if (m_hasProcedure) {
            painter->fillPath(path, Theme::inactiveTabBG);
            painter->fillRect(statusRect, hovered || is_selected ? Theme::background : Theme::sectionBackground);
        }else if(hovered || is_selected) {
            painter->fillPath(path, Theme::background);
		}

        const qreal splitY = upper ? kNumBoxH : (kCellH - kNumBoxH);
        painter->drawLine(QPointF(0.0, splitY), QPointF(kCellW, splitY));
        painter->drawPath(path);

    }
    else
    {
        if (hovered || is_selected) {
            painter->fillRect(numRect, Theme::background);
            painter->fillRect(statusRect, Theme::background);
        }

        if (m_hasProcedure) {
            painter->fillRect(numRect, Theme::inactiveTabBG);

        }

        painter->drawRect(statusRect);
        painter->drawRect(numRect);
    }

    QFont f = painter->font();
    f.setBold(true);
    painter->setFont(f);

    // drawing tooth number
    pen = painter->pen();
    pen.setColor(m_focused ? Theme::fontTurquoise : Theme::fontTurquoiseClicked);
    painter->setPen(pen);

    // circle/ellipse around the number when number > 48
    if (needsCircle(m_num))
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        QPen circlePen = pen;
        circlePen.setWidthF(2.0);
        circlePen.setCosmetic(true);
        painter->setPen(circlePen);
        painter->setBrush(Qt::NoBrush);

        const QFontMetrics fm(painter->font());
        const QRect textRect = fm.boundingRect(m_num);

        const qreal padX = 6.0;
        const qreal padY = 3.0;

        QRectF ellipseRect(
            numRect.center().x() - textRect.width() * 0.5 - padX,
            numRect.center().y() - textRect.height() * 0.5 - padY,
            textRect.width() + 2.0 * padX,
            textRect.height() + 2.0 * padY
        );

        ellipseRect = ellipseRect.intersected(numRect.adjusted(2.0, 2.0, -2.0, -2.0));

        painter->drawEllipse(ellipseRect);
        painter->restore();
    }

    painter->drawText(numRect.adjusted(kNumPadX, 0.0, -kNumPadX, 0.0), Qt::AlignCenter, m_num);

    // drawing status
    pen.setColor(Theme::fontTurquoiseClicked);
    painter->setPen(pen);

    f.setBold(is_selected);

    const QString statusText = m_stat_list.join(' ');
    QRectF r = statusRect.adjusted(kStatusPadX, kStatusPadY, -kStatusPadX, -kStatusPadY);

    f = fitFontToRectWrapped(statusText, f, r, 7);

    painter->setFont(f);
    painter->drawText(r, Qt::AlignCenter | Qt::TextWordWrap, statusText);

    // drawing tick lines
    QPen thick = painter->pen();
    thick.setWidthF(kThickW);
    painter->setPen(thick);

    if (m_idx == 7 || m_idx == 24) {
       painter->drawLine(QPointF(kCellW, 0.0), QPointF(kCellW, kCellH));
    } else if (m_idx == 8 || m_idx == 23) {
        painter->drawLine(QPointF(0.0, 0.0), QPointF(0.0, kCellH));
	}

    if (m_idx >= 0 && m_idx < 16) {
        painter->drawLine(QPointF(0.0, kCellH), QPointF(kCellW, kCellH));
    }
    else {
        painter->drawLine(QPointF(0.0, 0), QPointF(kCellW, 0));
    }

    painter->restore();
}
