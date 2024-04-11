#pragma once
#include <QString>

class Doctor;

class Nickname
{
	int m_hash{ -1 };
	
	QString name; //max length 20 guaranteed

	QString m_nickname;

	static int getRandomHash();

public:
	Nickname() {};
	Nickname(const std::string& fname, const std::string &lname, bool invisible);
	Nickname(const QString& nickname);

	//returns -1 if hash is invalid, 0 if is currentUser
	int hash() const { return m_hash; }
	const QString& nickname() const { return m_nickname; }
	QString parsedName() const;
	int isValid() const { return m_hash != -1; }
	int isCurrentUser() const { return m_hash == 0; }
	void rehashNickname();
	void setInvisible(bool invisible);
	bool isInvisible() const;
};
