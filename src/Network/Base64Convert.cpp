#include "Base64Convert.h"
#include <QString>

std::string Base64Convert::encode(const char* in, size_t in_len)
{
    return QByteArray(in, in_len).toBase64().toStdString();
}

std::string Base64Convert::decode(const std::string& input)
{
    return QByteArray::fromBase64(input.data()).toStdString();
}
