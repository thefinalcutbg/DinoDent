#include "SurfacePanel.h"

SurfacePanel::SurfacePanel(QWidget* parent)
	: QWidget(parent),
	  presenter(this)
{
	ui.setupUi(this);

	//QOpenGLWidget* gl = new QOpenGLWidget();
//	QSurfaceFormat format;

//	format.setSamples(16);
//	gl->setFormat(format);
//	ui.surfaceView->setViewport(gl);

	scene = new QGraphicsScene;
	ui.surfaceView->setScene(scene);
	scene->setBackgroundBrush(Qt::white);
	ui.surfaceView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	toothGraphic = new CPTooth;

	scene->addItem(toothGraphic);

	for (int i = 0; i < 5; i++) {
		polygon[i] = new ControlPanelPolygon(static_cast<ButtonPos>(i), this);
		scene->addItem(polygon[i]);
	}

	connect(ui.sideObturation, &QPushButton::clicked, [=] {buttonClicked(ButtonPos::side, MouseClick::leftClick); });
	connect(ui.sideCaries, &QPushButton::clicked, [=] {buttonClicked(ButtonPos::side, MouseClick::rightClick); });
}

SurfacePanel::~SurfacePanel()
{
}


void SurfacePanel::paintTooth(const Tooth* tooth)
{
	
	toothGraphic->setPixmap(painter.paintTooth(hint_creator.getToothHint(*tooth)));
}


SurfacePanelPresenter* SurfacePanel::getPresenter()
{
	return &presenter;
}

void SurfacePanel::showPanel(bool show)
{
	if (show && isHidden()) {
		this->show();
		return;
	}
	else if(!show){

		hide();
	}
}

void SurfacePanel::setLabels(std::array<std::string, 6> surfaceNames)
{
	for (int i = 0; i < 6; i++) {
		labels[i] = QString::fromStdString(surfaceNames[i]);
	}

	ui.sideLabel->setText(labels[5] + ":");
}

void SurfacePanel::setStatuses(std::array<std::string, 6> StatusNames)
{
	for (int i = 0; i < 6; i++) {
		statuses[i] = QString::fromStdString(StatusNames[i]);
	}
}

void SurfacePanel::setSideButtonsClicked(bool obturation, bool caries)
{
	ui.sideObturation->setChecked(obturation);
	ui.sideCaries->setChecked(caries);
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
		ui.surfaceInfoLabel->setText(labels[static_cast<int>(position)]);
		ui.statusInfoLabel->setText(statuses[static_cast<int>(position)]);
	}

}

void SurfacePanel::buttonClicked(ButtonPos position, MouseClick click)
{
	SurfaceType type = SurfaceType::obturation;
	if (click == MouseClick::rightClick) {
		type = SurfaceType::caries;
	}

	presenter.buttonClicked(position, type);

	if (position != ButtonPos::side)
	{
		ui.statusInfoLabel->setText(statuses[static_cast<int>(position)]);
	}

}
