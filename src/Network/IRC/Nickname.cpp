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


Nickname::Nickname(const std::string& fname, const std::string& lname, bool visible) :
	m_hash(0)
{

	m_nickname.reserve(24);

	name += fname.c_str();
	name += " ";
	name += lname.c_str();

	constexpr const int max = 21;

	if (name.length() > max) {

		name = lname.c_str();

		if (name.length() > max) {
			name.truncate(max);
		}
	}

	if (visible) {
		m_nickname += ']';
	}

	m_nickname += name;

	for (int i = 0; i < m_nickname.size(); i++) {
		if (m_nickname[i] == ' ') {
			m_nickname[i] = '_';
		}
	}

	m_nickname += QString::number(getRandomHash());

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

	bool visible = isVisible();

	m_nickname.clear();

	if (visible) {
		m_nickname += ']';
	}

	m_nickname += name;

	for (int i = 0; i < m_nickname.size(); i++) {
		if (m_nickname[i] == ' ') {
			m_nickname[i] = '_';
		}
	}

	m_nickname += QString::number(getRandomHash());

}

void Nickname::setVisible(bool visible)
{
	if (visible == isVisible()) return;

	if (visible) {
		m_nickname.insert(0, ']');
	}
	else {
		m_nickname.remove(0, 1);
	}

}

bool Nickname::isVisible() const
{
	return m_nickname.size() && m_nickname[0] == ']';
}
