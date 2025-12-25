#include "CommonIcon.h"

#include <QPixmap>
#include <QPainter>

#include <vector>

std::vector<QPixmap> s_pixmaps;
bool icon_arr_init = false;

const QPixmap& CommonIcon::getPixmap(CommonIcon::Type t)
{
    if (!icon_arr_init) {

        s_pixmaps = std::vector<QPixmap>(CommonIcon::MAX_SIZE, QPixmap());

        s_pixmaps[CommonIcon::NHIF] = QPixmap(":/icons/icon_nhif.png");
        s_pixmaps[CommonIcon::HIS] = QPixmap(":/icons/icon_his.png");
        s_pixmaps[CommonIcon::HISGRAY] = QPixmap(":/icons/icon_hisgray.png");
        s_pixmaps[CommonIcon::PHIF] = QPixmap(":/icons/icon_phif.png");
        s_pixmaps[CommonIcon::PATIENT] = QPixmap(":/icons/icon_user.png");
        s_pixmaps[CommonIcon::BDAY] = QPixmap(":/icons/icon_bday.png");
        s_pixmaps[CommonIcon::AMBLIST] = QPixmap(":/icons/icon_sheet.png");
        s_pixmaps[CommonIcon::PRESCR] = QPixmap(":/icons/icon_prescr.png");
        s_pixmaps[CommonIcon::PERIO] = QPixmap(":/icons/icon_periosheet.png");
        s_pixmaps[CommonIcon::TREATMENTPLAN] = QPixmap(":/icons/icon_plan.png");
        s_pixmaps[CommonIcon::INVOICE] = QPixmap(":/icons/icon_invoice.png");
        s_pixmaps[CommonIcon::CALENDAR] = QPixmap(":/icons/icon_calendar.png");
        s_pixmaps[CommonIcon::PRINT] = QPixmap(":/icons/icon_print.png");
        s_pixmaps[CommonIcon::DATABASE] = QPixmap(":/icons/icon_db.png");
        s_pixmaps[CommonIcon::REFRESH] = QPixmap(":/icons/icon_sync.png");
		s_pixmaps[CommonIcon::SIGNATURE] = QPixmap(":/icons/icon_sign.png");
        s_pixmaps[CommonIcon::CHECKED] = QPixmap(":/icons/icon_apply.png");
        icon_arr_init = true;
    }

    return s_pixmaps[t];
}

QPixmap CommonIcon::getIndicator(const QColor& color)
{
    if (!color.isValid()) return QPixmap();

    QPixmap px(64, 64);

    px.fill(Qt::transparent);

    QPainter painter(&px);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::PenStyle::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(QRect(10, 10, 44, 44));
    return px;
}
