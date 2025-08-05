#include "SignatureViewDialog.h"
#include "ui_SignatureViewDialog.h"

SignatureViewDialog::SignatureViewDialog(const std::vector<unsigned char>& bitmap, const std::string& signature)
    : QDialog(nullptr)
    , ui(new Ui::SignatureViewDialog)
{
    ui->setupUi(this);

    setWindowTitle("Преглед на подпис");

    ui->plainTextEdit->setPlainText(signature.c_str());

    QByteArray bytes(reinterpret_cast<const char*>(bitmap.data()),
                     static_cast<int>(bitmap.size()));

    QPixmap px;

    if (!px.loadFromData(bytes, "PNG")){
        ui->signatureLabel->setText("Изображението не може да бъде заредено");
        return;
    }

    ui->signatureLabel->setMinimumHeight(px.height() + 24);
    ui->signatureLabel->setMinimumWidth(px.width() + 24);
    ui->signatureLabel->setScaledContents(true);
    ui->signatureLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->signatureLabel->setAlignment(Qt::AlignCenter);
    ui->signatureLabel->setStyleSheet("background-color:white");
    ui->signatureLabel->setPixmap(px);
}

SignatureViewDialog::~SignatureViewDialog()
{
    delete ui;
}
