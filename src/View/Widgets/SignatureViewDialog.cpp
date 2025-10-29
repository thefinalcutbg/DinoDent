#include "SignatureViewDialog.h"
#include "ui_SignatureViewDialog.h"
#include "View/ModalDialogBuilder.h"

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

    ui->signatureLabel->setMinimumHeight(px.height()/2);
    ui->signatureLabel->setMinimumWidth(px.width()/2);
    ui->signatureLabel->setScaledContents(true);
    ui->signatureLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->signatureLabel->setAlignment(Qt::AlignCenter);
    ui->signatureLabel->setStyleSheet("background-color:white");
    ui->signatureLabel->setPixmap(px);

    connect(ui->removeSig, &QPushButton::clicked, [&]() {

        if(!ModalDialogBuilder::askDialog(
            "Сигурни ли сте, че искате да премахнете този подпис?")
        ) 
        {
            return;
		}

        m_for_removal = true;

        close();
    });
}

SignatureViewDialog::~SignatureViewDialog()
{
    delete ui;
}
