#include "Base64Convert.h"
#include <QString>

std::string Base64Convert::encode(const std::string& input)
{
    return QByteArray(input.data(), input.length()).toBase64().toStdString();
}

std::string Base64Convert::decode(const std::string& input)
{
    return QByteArray::fromBase64(input.data()).toStdString();
}
