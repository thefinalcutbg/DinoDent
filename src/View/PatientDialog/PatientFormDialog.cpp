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

    nameValidator = new QRegExpValidator(QRegExp(("[А-Я-а-я-a-z-A-Z- ]+")), this);
    ui.fNameEdit->QLineEdit::setValidator(nameValidator);
    ui.mNameEdit->QLineEdit::setValidator(nameValidator);
    ui.lNameEdit->QLineEdit::setValidator(nameValidator);

    phoneValidator = new QRegExpValidator(QRegExp("[0-9-+ ]+"), this);
    ui.phoneEdit->QLineEdit::setValidator(phoneValidator);

    connect(ui.typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [=](int index) { presenter->changePatientType(index + 1); ui.idLineEdit->QLineEdit::setFocus(); });

    connect(ui.okButton, &QPushButton::clicked, [=] { presenter->accept(); });
    connect(ui.idLineEdit, &QLineEdit::textEdited, [=]{ if(ui.idLineEdit->isValid()) presenter->searchDbForPatient(ui.typeComboBox->currentIndex()+1); });
    connect(ui.cityLineEdit, &QLineEdit::textChanged, [=] {presenter->cityChanged(); });


    patientFields[id] = ui.idLineEdit;
    patientFields[fname] = ui.fNameEdit;
    patientFields[mname] = ui.mNameEdit;
    patientFields[lname] = ui.lNameEdit;
    patientFields[city] = ui.cityLineEdit;
    patientFields[address] = ui.addressEdit;
    patientFields[hirbno] = ui.HIRBNoEdit;
    patientFields[phone] = ui.phoneEdit;

    for (auto& line : patientFields)
    {
        line->setErrorLabel(ui.errorLabel);
    }

    ui.birthEdit->setErrorLabel(ui.errorLabel);

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
    ui.idLineEdit->setReadOnly(editMode);
    ui.typeComboBox->disable(editMode);
    ui.idLineEdit->setFocus();
    ui.idLineEdit->selectAll();

    editMode ?
        setWindowTitle("Редактиране на данните на пациента")
        :
        setWindowTitle("Нов амбулаторен лист");


}

void PatientFormDialog::close()
{
    reject();
}

void PatientFormDialog::setLn4View(bool show)
{
    ui.sexCombo->setCurrentIndex(0);

}

void PatientFormDialog::setCodeInfo(const std::string& codeInfo)
{
    ui.codeLabel->setText(QString::fromStdString(codeInfo));
}

void PatientFormDialog::resetFields()
{
    ui.idLineEdit->reset();
    ui.idLineEdit->setValidAppearence(true);
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
    ui.idLineEdit->QLineEdit::setText(QString::fromStdString(patient.id));
    ui.sexCombo->setCurrentIndex(patient.sex);

    auto& date = patient.birth;
    ui.birthEdit->setDate(QDate(date.year, date.month, date.day));

    ui.fNameEdit->QLineEdit::setText(QString::fromStdString(patient.FirstName));
    ui.mNameEdit->QLineEdit::setText(QString::fromStdString(patient.MiddleName));
    ui.lNameEdit->QLineEdit::setText(QString::fromStdString(patient.LastName));

    ui.cityLineEdit->QLineEdit::setText(QString::fromStdString(patient.city));
    
    ui.addressEdit->QLineEdit::setText(QString::fromStdString(patient.address));
    ui.HIRBNoEdit->QLineEdit::setText(QString::fromStdString(patient.HIRBNo));
    ui.phoneEdit->QLineEdit::setText(QString::fromStdString(patient.phone));
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

AbstractDateEdit* PatientFormDialog::dateEdit()
{
    return ui.birthEdit;
}


