#include "PatientFormDialog.h"

PatientFormDialog::PatientFormDialog(PatientDialogPresenter* p, QWidget* parent)
    : QDialog(parent),
    presenter(p)
{
    ui.setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Нов амбулаторен лист");

    numValidator = new QRegExpValidator(QRegExp(("[0-9]+")), this);
    ui.idLineEdit->setValidator(numValidator);
    ui.HIRBNoEdit->setValidator(numValidator);

    nameValidator = new QRegExpValidator(QRegExp(("[А-Я-а-я-a-z-A-Z- ]+")), this);
    ui.fNameEdit->setValidator(nameValidator);
    ui.mNameEdit->setValidator(nameValidator);
    ui.lNameEdit->setValidator(nameValidator);

    phoneValidator = new QRegExpValidator(QRegExp("[0-9-+ ]+"), this);
    ui.phoneEdit->setValidator(phoneValidator);

    connect(ui.typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](int index) { presenter->changePatientType(index+1); });

    connect(ui.okButton, &QPushButton::clicked, [=] { presenter->accept(); });
    connect(ui.idLineEdit, &IdLineEdit::validIdEntered, [=]{presenter->searchDbForPatient(); });
    connect(ui.cityLineEdit, &QLineEdit::textChanged, [=] {presenter->cityChanged(); });


    patientFields[id] = ui.idLineEdit;
    patientFields[birthdate] = ui.birthEdit;
    patientFields[fname] = ui.fNameEdit;
    patientFields[mname] = ui.mNameEdit;
    patientFields[lname] = ui.lNameEdit;
    patientFields[city] = ui.cityLineEdit;
    patientFields[address] = ui.addressEdit;
    patientFields[hirbno] = ui.HIRBNoEdit;
    patientFields[phone] = ui.phoneEdit;

    presenter->setView(this);

}

void PatientFormDialog::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
    painter.end();
}

PatientFormDialog::~PatientFormDialog()
{

}

void PatientFormDialog::setEditMode(bool editMode)
{
    if (editMode)
    {
        ui.idLineEdit->disable(1);
        ui.typeComboBox->disable(1);
        setWindowTitle("Редактиране на данните на пациента");
    }
    else
    {
        ui.idLineEdit->disable(0);
        ui.typeComboBox->disable(0);
        setWindowTitle("Нов амбулаторен лист");
    }

}

void PatientFormDialog::close()
{
    reject();
}

void PatientFormDialog::setLn4View(bool show)
{
    ui.sexCombo->setCurrentIndex(0);

    if(show)
    {
        ui.birthEdit->show();
        ui.sexCombo->show();
        ui.label_7->show();
        ui.label_8->show();
    }
    else
    {
        ui.birthEdit->hide();
        ui.sexCombo->hide();
        ui.label_7->hide();
        ui.label_8->hide();
    }
}

void PatientFormDialog::setCodeInfo(const std::string& codeInfo)
{
    ui.codeLabel->setText(QString::fromStdString(codeInfo));
}

void PatientFormDialog::resetFields()
{
    ui.idLineEdit->reset();
    ui.idLineEdit->setAppearence(true);
    ui.birthEdit->reset();
    ui.fNameEdit->reset();
    ui.mNameEdit->reset();
    ui.lNameEdit->reset();
    ui.cityLineEdit->reset();
    ui.HIRBNoEdit->reset();
    ui.phoneEdit->reset();
    ui.addressEdit->reset();

    ui.codeLabel->setText("");
    ui.sexCombo->setCurrentIndex(0);

}

void PatientFormDialog::setPatient(const Patient& patient)
{
    QSignalBlocker blocker(this);

    ui.typeComboBox->setCurrentIndex(patient.type - 1);
    ui.idLineEdit->setText(QString::fromStdString(patient.id));
    ui.sexCombo->setCurrentIndex(patient.sex);

    auto& date = patient.birth;
    ui.birthEdit->setDate(QDate(date.year, date.month, date.day));

    ui.fNameEdit->setText(QString::fromStdString(patient.FirstName));
    ui.mNameEdit->setText(QString::fromStdString(patient.MiddleName));
    ui.lNameEdit->setText(QString::fromStdString(patient.LastName));

    ui.cityLineEdit->setText(QString::fromStdString(patient.city));
    
    ui.addressEdit->setText(QString::fromStdString(patient.address));
    ui.HIRBNoEdit->setText(QString::fromStdString(patient.HIRBNo));
    ui.phoneEdit->setText(QString::fromStdString(patient.phone));
}

Patient PatientFormDialog::getPatient()
{
    auto birthDate = ui.birthEdit->date();

    return Patient
    {
        ui.typeComboBox->currentIndex() + 1,
        ui.idLineEdit->text().toStdString(),
        Date(birthDate.day(), birthDate.month(), birthDate.year()),
        bool(ui.sexCombo->currentIndex()),
        ui.fNameEdit->text().toStdString(),
        ui.mNameEdit->text().toStdString(),
        ui.lNameEdit->text().toStdString(),
        ui.cityLineEdit->text().toStdString(),
        ui.addressEdit->text().toStdString(),
        ui.HIRBNoEdit->text().toStdString(),
        ui.phoneEdit->text().toStdString(),
    };
}

AbstractLineEdit* PatientFormDialog::lineEdit(PatientField field)
{
    return patientFields[field];
}


