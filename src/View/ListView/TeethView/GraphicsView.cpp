#include "GraphicsView.h"
#include <QApplication>
#include <QDebug>

GraphicsView::GraphicsView(QWidget *parent)
	: QGraphicsView(parent), gl{nullptr}
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setCacheMode(QGraphicsView::CacheBackground);
	setDragMode(QGraphicsView::RubberBandDrag);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	
	gl = new QOpenGLWidget();
	
	QSurfaceFormat format;

	format.setSamples(4);

	gl->setFormat(format);
	setViewport(gl);
	gl->setUpdateBehavior(QOpenGLWidget::UpdateBehavior::NoPartialUpdate);
	
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	QGraphicsView::mousePressEvent(event);

	if(event->button() == Qt::LeftButton && 
		!QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
		QGraphicsView::mousePressEvent(event);
}


GraphicsView::~GraphicsView()
{
}
