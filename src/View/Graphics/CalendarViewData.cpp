﻿#include "CalendarViewData.h"
#include <QPainter>
#include <QPainterPath>
#include "View/Theme.h"
#include <QApplication>

CalendarViewData::CalendarViewData()
{}

void CalendarViewData::setEvents(const std::vector<CalendarEvent>& eventsList, const CalendarEvent& clipboardEvent)
{
	coordinatesMap.clear();
	current_entity_hover = nullptr;
	m_clipboardEventText = clipboardEvent.summary.c_str();

	if (clipboardEvent.id.size() && m_clipboardEventText.isEmpty()) {
		m_clipboardEventText = "???";
	}

	for (int i = 0; i < eventsList.size(); i++)
	{
		auto& event = eventsList[i];
		
		if (event.id == clipboardEvent.id) { //wont paint the clipboard event
			continue;
		}
		
		auto entity = std::make_shared<EventEntity>();

		entity->hasPatient = event.patientBirth.size();
		
		if (entity->hasPatient) {

			entity->text += "• ";
		}

		entity->text += event.summary.c_str();

		if (event.description.size()) {
			entity->text += "\n";
			entity->text += event.description.c_str();
		}

		if (entity->text.isEmpty()) {
			entity->text = "???";
		}

		entity->span = (event.start.secsTo(event.end) / 60 / 15);
		
		entity->event_list_index = i;

		//for events under 15 minutes
		if (!entity->span) {
			entity->span = 1;
		}

		entity->column = event.start.date().dayOfWeek() - 1;

		entity->row = QTime(0, 0, 0).secsTo(event.start.time()) / 60 / 15;

		for (int y = entity->row; y < entity->row + entity->span; y++) {

			int key = getEventKey(entity->column, y);

			coordinatesMap[key] = entity;
		}

	}
}

void CalendarViewData::setCellSize(int column, int cell_width, int cell_height)
{
	for (auto& [key, ptr] : coordinatesMap) {
		
		if (ptr->column == column) {
			ptr->setCellSize(cell_width, cell_height);
		}
	}
}


QPixmap CalendarViewData::requestPixmap(int column, int row) const
{
	auto entity = getEntity(column, row);

	if (!entity) {
		return QPixmap();
	}

	return entity->getPixmapPart(row);
}

int CalendarViewData::eventListIndex(int column, int row) const
{
	auto key = getEventKey(column, row);

	if (coordinatesMap.count(key)) {
		return coordinatesMap.at(key)->event_list_index;
	}

	return -1;
}

bool CalendarViewData::hasPatient(int row, int column) const
{
	auto key = getEventKey(column, row);

	if (coordinatesMap.count(key)) {
		return coordinatesMap.at(key)->hasPatient;
	}

	return false;
}

std::vector<std::pair<int, int>> CalendarViewData::setHovered(int column, int row)
{
	std::vector<std::pair<int, int>> result;
	
	auto newHoveredEntity = getEntity(column, row);

	//hovered over empty cell
	if (!newHoveredEntity) {
		
		//hovering out of entity to empty cell
		if (current_entity_hover)
		{
			//we have to de-hover the already hovered entity
			result = current_entity_hover->getSpan();
			current_entity_hover->setHovered(false);
			current_entity_hover = nullptr;
		}

		return result;
	}


	//no need to update anything - returning empty result
	if (newHoveredEntity == current_entity_hover) {
		return result;
	}

	//going from one entity to another
	if (current_entity_hover) {
		//adding the span of the last
		result = current_entity_hover->getSpan();

		current_entity_hover->setHovered(false);
	}

	//hovering from one entity to another:
	current_entity_hover = newHoveredEntity;

	current_entity_hover->setHovered(true);

	for (auto& idx : newHoveredEntity->getSpan()) {
		result.push_back(idx);
	}

	return result;
}

CalendarViewData::EventEntity* CalendarViewData::getEntity(int column, int row)
{

	auto key = getEventKey(column, row);

	if (!coordinatesMap.count(key)) return nullptr;

	return coordinatesMap.at(key).get();
}

const CalendarViewData::EventEntity* const CalendarViewData::getEntity(int column, int row) const
{
	auto key = getEventKey(column, row);

	if (!coordinatesMap.count(key)) return nullptr;

	return coordinatesMap.at(key).get();
}


void CalendarViewData::EventEntity::setCellSize(int width, int height)
{
	cell_width = width;
	cell_height = height;
	
	paintPixmap();
}

void CalendarViewData::EventEntity::paintPixmap()
{
	int eventHeight = cell_height * span;

	px = QPixmap(cell_width*pixelRatio, eventHeight*pixelRatio);
	px.setDevicePixelRatio(pixelRatio);
	px.fill(Qt::transparent);

	QPainter p(&px);

	p.setRenderHint(QPainter::Antialiasing);
	
	auto cellRect = QRect(0, 0, cell_width, eventHeight);

	p.setPen(QColor(245, 245, 245));
	p.drawRect(cellRect);
	
	if (row % 4 == 0) {
		p.setPen(QColor(224, 224, 224));
		p.drawLine(cellRect.topLeft(), cellRect.topRight());
	}

	QPainterPath path;
	path.addRoundedRect(QRectF(2,2,cell_width-4, eventHeight-4), 7, 7);

	
	p.fillPath(path, m_hovered ? Theme::inactiveTabBGHover : Theme::inactiveTabBG);
	p.setPen(Theme::mainBackgroundColor);
	p.drawPath(path);
	QFont font = p.font();
	font.setBold(true);

	p.setFont(font);

	p.setPen(Theme::fontTurquoise);

	QRect textRect(5, 4, cell_width-10, eventHeight-6);
	
	p.setRenderHint(QPainter::RenderHint::TextAntialiasing);

	p.drawText(textRect, text);
}

QPixmap CalendarViewData::EventEntity::getPixmapPart(int row) const
{
	int section = row - this->row;

	int pointY = cell_height * section;

	QRectF sourceRect(0, pixelRatio* pointY, pixelRatio * cell_width, pixelRatio * cell_height);

	QRectF target(0, 0, pixelRatio * cell_width, pixelRatio * cell_height);

	QPixmap result(pixelRatio * cell_width, pixelRatio * cell_height);

	result.fill(Qt::transparent);
	
	QPainter p(&result);

	p.drawPixmap(target, this->px, sourceRect);

	return result;
}

void CalendarViewData::EventEntity::setHovered(bool hovered)
{
	m_hovered = hovered;
	paintPixmap();
}

std::vector<std::pair<int, int>> CalendarViewData::EventEntity::getSpan() const
{
	std::vector<std::pair<int, int>> result;

	for (int i = row; i < row + span; i++) {
		result.push_back(std::make_pair(column, i));
	}

	return result;
}
