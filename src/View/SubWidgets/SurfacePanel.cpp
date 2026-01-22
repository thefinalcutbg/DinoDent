#include "SurfacePanel.h"

#include <QPainter>

#include "Presenter/SurfacePanelPresenter.h"
#include "View/Graphics/ToothPainter.h"
#include "View/Theme.h"

SurfacePanel::SurfacePanel(QWidget* parent)
	: QWidget(parent), presenter(NULL)
{
	ui.setupUi(this);

	ui.notesButton->setIcon(QIcon(":/icons/icon_notes.png"));
	ui.notesButton->setGraphicsEffect(nullptr);
	scene = new QGraphicsScene;
	ui.surfaceView->setScene(scene);
	scene->setBackgroundBrush(Qt::white);
	ui.surfaceView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	toothGraphic = new CPTooth;
	//ui.sideBox->setStyleSheet("color: rgb(2, 127, 128); font-weight: bold;");

	scene->addItem(toothGraphic);

	for (int i = 0; i < 5; i++) {
		polygon[i] = new ControlPanelPolygon(static_cast<ButtonPos>(i), this);
		scene->addItem(polygon[i]);
	}

	polygon_border = new PolygonBorder();

	scene->addItem(polygon_border);

	connect(ui.sideButton, &QPushButton::clicked, [this] { presenter->buttonClicked(ButtonPos::side, SurfaceClick::leftClick); });
	connect(ui.sideButton, &StatusButton::rightClicked, [this] { presenter->buttonClicked(ButtonPos::side, SurfaceClick::rightClick); });
	connect(ui.notesButton, &QPushButton::clicked, [this] { presenter->notesClicked(); });


	
}

void SurfacePanel::drawFocused(bool focused)
{
	polygon_border->drawFocused(focused);

	m_focused = focused;
	update();
}

SurfacePanel::~SurfacePanel()
{
}

void SurfacePanel::setPresenter(SurfacePanelPresenter* presenter)
{
	this->presenter = presenter;
	presenter->setView(this);
}

void SurfacePanel::paintTooth(const ToothPaintHint& tooth, bool hasNotes)
{
	toothGraphic->setPixmap(ToothPainter::getOcclusal(tooth));
	ui.notesButton->setIconMode(hasNotes ? IconButton::COLOR : IconButton::MONOCHROME);
}

void SurfacePanel::hidePanel(bool hidden)
{
	setHidden(hidden);
}

void SurfacePanel::setLabels(std::array<std::string, 6>& surfaceNames)
{
	for (int i = 0; i < 6; i++) {
		labels[i] = QString::fromStdString(surfaceNames[i]);
	}

	ui.sideBox->setText(labels[5]+":");
}

void SurfacePanel::setStatuses(std::array<std::string, 6>& StatusNames)
{
	for (int i = 0; i < 6; i++) {
		statuses[i] = QString::fromStdString(StatusNames[i]);
	}

	ui.sideButton->setText(StatusNames[5].c_str());
	ui.sideButton->setChecked(!ui.sideButton->text().contains("Инт"));
	ui.sideButton->pathology = ui.sideButton->text() != "Възстановяване";
}

void SurfacePanel::buttonHovered(ButtonPos position, Hover hoverState)
{
	if (hoverState == Hover::out)
	{
		ui.surfaceInfoLabel->setText("");
		ui.statusInfoLabel->setText("");
	}
	else
	{
		ui.surfaceInfoLabel->setText(labels[static_cast<int>(position)] + ":");
		ui.statusInfoLabel->setText(statuses[static_cast<int>(position)]);
	}

}

void SurfacePanel::buttonClicked(ButtonPos position, MouseClick click)
{
	if (click == MouseClick::rightClick)
		presenter->buttonClicked(position, SurfaceClick::rightClick);
	else if (click == MouseClick::leftClick)
		presenter->buttonClicked(position, SurfaceClick::leftClick);

	ui.statusInfoLabel->setText(statuses[static_cast<int>(position)]);
}


void SurfacePanel::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	painter.setPen(QPen(m_focused ? Theme::mainBackgroundColor : Theme::border));

	/*
	painter.drawRoundedRect(
		ui.frame->x(),
		ui.frame->y(),
		ui.frame->width(),
		ui.frame->height(),
		Theme::radius / 2,
		Theme::radius / 2
	);
	*/
}
