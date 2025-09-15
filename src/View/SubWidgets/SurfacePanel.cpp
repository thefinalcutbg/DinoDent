#include "SurfacePanel.h"

#include <QPainter>

#include "Presenter/SurfacePanelPresenter.h"
#include "View/Graphics/ToothPainter.h"
#include "View/Theme.h"

SurfacePanel::SurfacePanel(QWidget* parent)
	: QWidget(parent), presenter(NULL)
{
	ui.setupUi(this);

	scene = new QGraphicsScene;
	ui.surfaceView->setScene(scene);
	scene->setBackgroundBrush(Qt::white);
	ui.surfaceView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	toothGraphic = new CPTooth;
	//ui.sideBox->setStyleSheet("color: rgb(2, 127, 128); font-weight: bold;");

	ui.sideCaries->pathology = true;
	ui.sideNonCaries->pathology = true;

	scene->addItem(toothGraphic);

	for (int i = 0; i < 5; i++) {
		polygon[i] = new ControlPanelPolygon(static_cast<ButtonPos>(i), this);
		scene->addItem(polygon[i]);
	}

	polygon_border = new PolygonBorder();

	scene->addItem(polygon_border);

	auto buttonOrder = {
		std::make_pair(Dental::StatusType::Restoration, ui.sideObturation),
		std::make_pair(Dental::StatusType::Caries, ui.sideCaries),
		std::make_pair(Dental::StatusType::DefectiveRestoration, ui.sideDefect),
		std::make_pair(Dental::StatusType::NonCariesLesion, ui.sideNonCaries)
	};

	for (auto& s : buttonOrder) {
		connect(s.second, &QPushButton::clicked, this, [=, this] { presenter->sideButtonClicked(s.first);  });
	}

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

void SurfacePanel::paintTooth(const ToothPaintHint& tooth)
{
	
	toothGraphic->setPixmap(ToothPainter::getOcclusal(tooth));
}

void SurfacePanel::hidePanel(bool hidden)
{
	setHidden(hidden);
}

void SurfacePanel::setLabels(std::array<std::string, 6> surfaceNames)
{
	for (int i = 0; i < 6; i++) {
		labels[i] = QString::fromStdString(surfaceNames[i]);
	}

	ui.sideBox->setText(labels[5]+":");
}

void SurfacePanel::setStatuses(std::array<std::string, 6> StatusNames)
{
	for (int i = 0; i < 6; i++) {
		statuses[i] = QString::fromStdString(StatusNames[i]);
	}
}

void SurfacePanel::setSideButtonsClicked(bool restoration, bool caries, bool defectRes, bool nonCaries)
{
	ui.sideObturation->setChecked(restoration);
	ui.sideCaries->setChecked(caries);
	ui.sideDefect->setChecked(defectRes);
	ui.sideNonCaries->setChecked(nonCaries);
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

	painter.drawRoundedRect(
		ui.frame1->x(),
		ui.frame1->y(),
		ui.frame1->width(),
		ui.frame1->height(),
		Theme::radius / 2,
		Theme::radius / 2
	);

	//painter.drawRoundedRect(
	//	ui.frame2->x(),
	//	ui.frame2->y(),
	//	ui.frame2->width(),
	//	ui.frame2->height(),
	//	Theme::radius / 2,
	//	Theme::radius / 2
	//);
}
