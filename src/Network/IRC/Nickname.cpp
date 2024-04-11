#include "Nickname.h"

#include <random>

#include "Model/UserStructs.h"
#include "Model/Validators/LpkValidator.h"


int Nickname::getRandomHash()
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed
	std::uniform_int_distribution<> distr(1, 999); // 0 is reserved for current user
	return distr(gen);
}


Nickname::Nickname(const std::string& fname, const std::string& lname, bool invisible) :
	m_hash(0)
{

	m_nickname.reserve(24);

	name += fname.c_str();
	name += " ";
	name += lname.c_str();

	constexpr const int max = 20;

	if (name.length() > max) {

		name = lname.c_str();

		if (name.length() > max) {
			name.truncate(max);
		}
	}

	m_nickname += name;

	m_nickname.replace(' ', '_');

	m_nickname += QString::number(getRandomHash());

	if (invisible) {
		m_nickname += '_';
	}
}

Nickname::Nickname(const QString& nickname) : m_nickname(nickname)
{

	QString hashStr;

	for (const QChar c : m_nickname) {

		if (c.isDigit()) {
			hashStr += c;
		}
		else if (c == '_') {
			name += ' ';
		}
		else {
			name += c;
		}
	}

	if (isInvisible()) {
		name.truncate(name.size() - 1);
	}

	if (hashStr.isEmpty()) return;

	m_hash = hashStr.toInt();

	if (m_hash < 0 || m_hash > 999) {
		m_hash = -1;
		m_nickname.clear();
		name.clear();
	}
}

QString Nickname::parsedName() const
{
	if (!isValid()) return QString();

	return "д-р " + name;
}

void Nickname::rehashNickname()
{
	if (!isCurrentUser()) return;

	bool invisible = isInvisible();

	m_nickname = name;

	m_nickname.replace(' ', '_');

	m_nickname += QString::number(getRandomHash());

	if (invisible) {
		m_nickname += '_';
	}

}

void Nickname::setInvisible(bool invisible)
{
	if (invisible == isInvisible()) return;

	if (invisible) {
		m_nickname.push_back('_');
	}
	else {
		m_nickname.truncate(m_nickname.size() - 1);
	}

}

bool Nickname::isInvisible() const
{
	return m_nickname.size() && m_nickname.back() == '_';
}
