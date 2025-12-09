#include "FreeFunctions.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

std::string FreeFn::formatDouble(const double& price)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << price;

    return stream.str();
}

std::string FreeFn::leadZeroes(long long num, int totalLength)
{
    std::string formated;
    formated.reserve(totalLength);
    std::string number(std::to_string(num));

    if (totalLength < number.length()) return number;

    for (int i = 0; i < totalLength - number.length(); i++) {
        formated += "0";
    }

    formated += number;

    return formated;
}

std::string FreeFn::leadZeroes(const std::string& number, int totalLength)
{

    std::string formated;
    formated.reserve(totalLength);

    if (totalLength < number.length()) return number;

    for (int i = 0; i < totalLength - number.length(); i++) {
        formated += "0";
    }

    return formated + number;
}

std::string FreeFn::getFile(const std::string& filepath)
{

    std::ifstream t(filepath);
    return std::string((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());

}

std::string FreeFn::removeTrailing(double a)
{
    auto result = std::to_string(a);

    while (result.back() == '0' && result.size()) result.pop_back();

    if (result.back() == '.') result.pop_back();

    return result;
}

#include <QUuid>

std::string FreeFn::getUuid()
{
    return QUuid::createUuid().toString(QUuid::StringFormat::Id128).toStdString();
}

#include <QDateTime>

std::string FreeFn::getTimeStampLocal()
{
    return QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
}

std::string FreeFn::getTimeStampUTC()
{
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
}

std::string FreeFn::UTCToLocal(const std::string& timeStamp)
{
    if (timeStamp.size() < 19) return std::string();

    auto temp = timeStamp.substr(0, 19);

    QDateTime datetime = QDateTime::fromString(temp.c_str(), "yyyy-MM-ddTHH:mm:ss");
    datetime.setTimeSpec(Qt::TimeSpec::UTC);
    return datetime.toLocalTime().toString(Qt::ISODate).toStdString();
}

std::string FreeFn::LocalToUTC(const std::string& timeStamp)
{
    QDateTime datetime = QDateTime::fromString(timeStamp.c_str(), "yyyy-MM-ddTHH:mm:ss");
    datetime.setTimeSpec(Qt::TimeSpec::LocalTime);
    return datetime.toUTC().toString(Qt::ISODate).toStdString();
}

std::string FreeFn::escapeXml(const std::string& data)
{
    std::string result;
    result.reserve(data.size());

    for (size_t pos = 0; pos != data.size(); ++pos) {
        switch (data[pos]) {
        case '&':  result.append("&amp;");       break;
        case '\"': result.append("&quot;");      break;
        case '\'': result.append("&apos;");      break;
        case '<':  result.append("&lt;");        break;
        case '>':  result.append("&gt;");        break;
        default:   result.append(&data[pos], 1); break;
        }
    }
    
    return result;
}

std::string FreeFn::toUpper(const std::string& src)
{
    return QString(src.c_str()).toUpper().toStdString();
}


bool FreeFn::contains(const std::string& parent, const std::string& child)
{
    return QString(parent.c_str()).contains(child.c_str());
}

std::string FreeFn::getPatientName(const std::string& fname, const std::string& mname, const std::string& lname)
{
	std::string name = fname + " ";
	
	if (!mname.empty()) {
		name += mname + " ";
	}

    name += lname;

	return name;
}


std::string FreeFn::getPhoneFromString(const std::string& text)
{
    QString qtext = QString::fromStdString(text);

    static const QRegularExpression re(R"((\+?\d[\d\s\-/]{8,}\d))");

    QRegularExpressionMatch match = re.match(qtext);
    if (!match.hasMatch()) {
        return std::string();
    }

    QString rawPhone = match.captured(1).trimmed();
    return rawPhone.toStdString();
}