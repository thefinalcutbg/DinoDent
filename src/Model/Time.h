#pragma once
#include <string>

class Date;

class Time {

public:
	int hour{ 0 };
	int minutes{ 0 };
	int sec{ 0 };

	Time(){}

	Time(int h, int m, int s) : hour(h), minutes(m), sec(s) {}

	Time(const std::string& ISO8601);

	std::string to8601(const Date& date, char separator = 'T') const;

	static Time currentTime();

};