#include "GraphicsView.h"
#include <QApplication>
#include <QDesktopWidget>

#include <QDebug>

GraphicsView::GraphicsView(QWidget *parent)
	: QGraphicsView(parent), gl{nullptr}
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	setDragMode(QGraphicsView::RubberBandDrag);

	gl = new QOpenGLWidget();
	QSurfaceFormat format;

	format.setSamples(4);

	gl->setFormat(format);

	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setViewport(gl);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	QGraphicsView::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
		QGraphicsView::mousePressEvent(event);
}


GraphicsView::~GraphicsView()
{
}
