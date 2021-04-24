#include "GraphicsView.h"


GraphicsView::GraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QOpenGLWidget* gl = new QOpenGLWidget();
	QSurfaceFormat format;

	format.setSamples(16);
	gl->setFormat(format);
	setViewport(gl);

	setDragMode(QGraphicsView::RubberBandDrag);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	QGraphicsView::mousePressEvent(event);
	QGraphicsView::mousePressEvent(event);
}

GraphicsView::~GraphicsView()
{
}
