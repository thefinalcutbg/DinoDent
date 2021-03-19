#pragma once
#include "AbstractValidator.h"
#include <QString>
#include <QFile>
#include "Model/Date.h"
#include <sstream>
#include <fstream>
#include <unordered_map>



class EgnValidator : public Validator
{
	bool dateCheck(int& d, int& m, int& y);
public:
	bool validate(const std::string& text);
};


class Ln4Validator : public Validator
{
public:
	bool validate(const std::string& text);
};

class HIRBNoValidator : public Validator
{
public:
	bool validate(const std::string& text);
};

class NameValidator : public Validator
{
	QString letters;
public:
	NameValidator();
	bool validate(const std::string& text);
};

class CityValidator : public Validator
{
	std::unordered_map<std::string, bool> cityCheckMap;
public:
	CityValidator();
	bool validate(const std::string& text);
};

class BirthValidator : public Validator
{
	Date defaultDate;

public:
	BirthValidator();
	bool validate(const std::string& text);
};

