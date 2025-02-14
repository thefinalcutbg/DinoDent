#include "PixmapLabel.h"

PixmapLabel::PixmapLabel(QWidget* parent) :
    QLabel(parent)
{
    this->setMinimumSize(1, 1);
    setScaledContents(false);
}

void PixmapLabel::setPixmap(const QPixmap& p)
{
    pix = p;
    QLabel::setPixmap(scaledPixmap());
}

int PixmapLabel::heightForWidth(int width) const
{
    return pix.isNull() ? this->height() : ((qreal)pix.height() * width) / pix.width();
}

QSize PixmapLabel::sizeHint() const
{
    int w = this->width();
    return QSize(w, heightForWidth(w));
}

QPixmap PixmapLabel::scaledPixmap() const
{
    auto scaled = pix.scaled(
        this->size() * devicePixelRatioF(), 
        Qt::KeepAspectRatio, 
        Qt::SmoothTransformation); 
        scaled.setDevicePixelRatio(devicePixelRatioF()); 

    return scaled;
}

bool PixmapLabel::pixmapIsNull() const
{
    return pix.isNull();
}

void PixmapLabel::resizeEvent(QResizeEvent*)
{
    if (!pix.isNull())
        QLabel::setPixmap(scaledPixmap());
}
