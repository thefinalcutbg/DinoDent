#include "GraphicsView.h"
#include <QApplication>
#include <QDebug>
#include <View/Theme.h>
#include <QPainter>

GraphicsView::GraphicsView(QWidget *parent)
	: QGraphicsView(parent), gl{nullptr}
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

void GraphicsView::paintEvent(QPaintEvent* event)
{
	QGraphicsView::paintEvent(event);
/*
	QPainter painter(viewport());

	auto pathFill = Theme::getHalfCurvedPath(11, height());

	painter.fillRect(QRect(0,0,10,height()), Theme::background);

	auto path = Theme::getHalfCurvedPath(width(), height());

	painter.fillPath(pathFill, Theme::sectionBackground);

	painter.setPen(QPen(Theme::border));

	painter.drawPath(path);

	painter.end();
	*/
}

GraphicsView::~GraphicsView()
{
}

void GraphicsView::wheelEvent(QWheelEvent* event)
{
	event->ignore();
}