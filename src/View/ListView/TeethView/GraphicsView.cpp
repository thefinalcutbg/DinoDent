#include "GraphicsView.h"


GraphicsView::GraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	gl = new QOpenGLWidget();
	QSurfaceFormat format;

	format.setSamples(4);

	gl->setFormat(format);
//	gl->setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setViewport(gl);

	setDragMode(QGraphicsView::RubberBandDrag);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	QGraphicsView::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
		QGraphicsView::mousePressEvent(event);
}

void GraphicsView::focusOutEvent(QFocusEvent* event)
{
	emit focusLost();
	QWidget::focusOutEvent(event);
}

GraphicsView::~GraphicsView()
{
}
