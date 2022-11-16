#include "NameEdit.h"

QString letters{ "абвгдежзийклмнопрстуфхцчшщъьюя " };
QString capletters{ "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЮЯ " };

NameEdit::NameEdit(QWidget* parent) : LineEdit(parent)
{
    connect(this, &QLineEdit::textEdited, [=] {makeFirstLetterCapital(); });
}

QString NameEdit::reformat(QString text)
{
    QString& name = text;

    if (name.isEmpty() || !isValid()) return this->text();

    //Formats the name:
//Removes spaces and dashes at the beginning and at the end of the name:

    while (name[name.size() - 1] == ' ' || name[name.size() - 1] == ' ')
    {
        name.remove(name.size() - 1, 1);
    }
    while (name[0] == ' ' || name[0] == '-')
    {
        name.remove(0, 1);
    }

    //makese it all lowercase, and formats the dashes:
    for (int i = 0; i < name.size(); i++)
    {
        if (name[i] == ' ') name[i] = ' ';

        for (int y = 0; y < 31; y++)
        {
            if (name[i] == capletters[y])name[i] = letters[y];
        }
    }

    //formats the dashes between the names:
    for (int i = 0; i < name.size(); i++)
    {
        if (name[i] == ' ' && name[i + 1] == ' ') { name.remove(i + 1, 1); i--; }
    }

    //makes the first letter capital:
    for (int y = 0; y < 32; y++)
    {
        if (name[0] == letters[y]) { name[0] = capletters[y]; break; }
    }
    // capitalizes the letter after the dash:
    for (int i = 0; i < name.size(); i++)
    {
        if (name[i] == ' ')
        {
            for (int y = 0; y < 32; y++)
            {
                if (name[i + 1] == letters[y]) { name[i + 1] = capletters[y]; break; }
            }
        }

    }

    return name;
}

void NameEdit::makeFirstLetterCapital()
{
    auto name = text();

    if (name.length() != 1) return;

    for (int i = 0; i < 30; i++)
    {
        if (name[0] == letters[i])
        {
            name[0] = capletters[i];
            QLineEdit::setText(name);
        }
    }
  
}

void NameEdit::reformat()
{
    QLineEdit::setText(reformat(QLineEdit::text()));
}


