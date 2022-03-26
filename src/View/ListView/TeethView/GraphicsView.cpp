#include "GraphicsView.h"
#include <QApplication>
#include <QDebug>
#include <View/Theme.h>
#include <QPainter>

GraphicsView::GraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
	
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setCacheMode(QGraphicsView::CacheBackground);
	setDragMode(QGraphicsView::RubberBandDrag);

	setStyleSheet("background: transparent");
	
	setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::FullViewportUpdate);
	
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

void GraphicsView::wheelEvent(QWheelEvent* event)
{
	event->ignore();
}