#include "PracticeDoctorSettings.h"
#include "Presenter/PracticeDoctorSettingsPresenter.h"
#include <QPainter>

void PracticeDoctorSettings::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

constexpr int specIdxSize = 5;
constexpr const char* specialties[specIdxSize]{
	"", "64", "61", "62", "68"
};

PracticeDoctorSettings::PracticeDoctorSettings(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (auto& specialty : specialties)
	{
		ui.specialtyCombo->addItem(specialty);
	}

	ui.adminCheck->setEnabled(false);
	ui.specialtyCombo->setEnabled(false);

	ui.doctorList->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(ui.removeDoctor, &QPushButton::clicked, [=]{ presenter->deleteDoctor(); });
	connect(ui.addDoctor, &QPushButton::clicked, [=] {presenter->addDoctor();});
	connect(ui.doctorList, &QListWidget::currentRowChanged, [=](int row) {presenter->indexChanged(row);});
	connect(ui.adminCheck, &QCheckBox::stateChanged, [=](int state) { presenter->setAdminPrivilege(state);});
	connect(ui.specialtyCombo, &QComboBox::currentIndexChanged, [=](int index) {
		presenter->setDoctorNhifSpecialty(static_cast<NhifSpecialty>(index)); 
		}
	);
}

void PracticeDoctorSettings::setDoctorList(const std::vector<PracticeDoctor>& doctors)
{
	ui.doctorList->clear();

	ui.adminCheck->setEnabled(doctors.size());
	ui.specialtyCombo->setEnabled(doctors.size());

	for (auto doctor : doctors)
	{
		QString postfix = doctor.admin ? u8" (администратор)" : "";
		ui.doctorList->addItem(QString::fromStdString(doctor.name) + postfix);
	}

	auto count = ui.doctorList->count();

	if (count) {
		ui.doctorList->setCurrentRow(count - 1);
	}
}

void PracticeDoctorSettings::setPresenter(PracticeDoctorSettingsPresenter* presenter)
{
	this->presenter = presenter;
}

void PracticeDoctorSettings::setDoctorProperties(bool admin, NhifSpecialty specialty)
{
	ui.adminCheck->show();
	ui.specialtyCombo->show();

	//recursion guards
	QSignalBlocker blocker(ui.adminCheck); 
	QSignalBlocker blocker2(ui.specialtyCombo);

	ui.adminCheck->setChecked(admin);
	ui.specialtyCombo->setCurrentIndex(static_cast<int>(specialty));

}


void PracticeDoctorSettings::hideDoctorProperties()
{
	ui.adminCheck->hide();
	ui.specialtyCombo->hide();
	ui.label->hide();
}

void PracticeDoctorSettings::replaceCurrentItem(const PracticeDoctor& item)
{
	QString postfix = item.admin ? u8" (администратор)" : "";
	ui.doctorList->currentItem()->setText(QString::fromStdString(item.name) + postfix);
}

PracticeDoctorSettings::~PracticeDoctorSettings()
{
}
