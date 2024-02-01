#include "NameEdit.h"


NameEdit::NameEdit(QWidget* parent) : LineEdit(parent)
{
    connect(this, &QLineEdit::textEdited, [&] {makeFirstLetterCapital(); });
}

QString NameEdit::reformat(QString text)
{
    QString& name = text;
    
    return name;
}

void NameEdit::makeFirstLetterCapital()
{
    auto name = text();

    if (name.isEmpty()) return;

    if (name[0].isLower())
    {
        name[0] = name[0].toUpper();
        QLineEdit::setText(name);
    }

    for (int i = 1; i < name.size(); i++)
    {
        if (!name[i - 1].isLetter() && 
            name[i].isLetter() &&
            name[i].isLower()
            )
        {
            name[i] = name[i].toUpper();
            QLineEdit::setText(name);
            break;
        }
    }
 
  
}

void NameEdit::reformat()
{
    QLineEdit::setText(reformat(QLineEdit::text()));
}


