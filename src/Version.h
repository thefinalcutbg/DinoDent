#pragma once
#include <string>

struct Version {

	static constexpr unsigned dbVersion() { return 9;}

	static constexpr Version current() {

		return {
			.MAJOR = 1, 
			.MINOR = 2, 
			.REVISION = 1
		};
	}

	const unsigned MAJOR{ 0 }, MINOR{ 0 }, REVISION{ 0 };

	static Version fromStr(const std::string& v) {

		std::string version;

		unsigned int arr[3]{ 0,0,0 };

		int index = 0;

		for (auto ch : v)
		{
			if (!std::isdigit(ch)) {

				if (ch != '.') return { 0,0,0 };

				if (index == 2) break;

				arr[index++] = std::stoi(version);

				version.clear();

				continue;
			}

			version.push_back(ch);
		}

		arr[index] = std::stoi(version);

		return Version{ arr[0], arr[1], arr[2] };
	}

	bool isLessThan(const Version& other) const
	{	
		if (MAJOR < other.MAJOR ) return true;
		if (MAJOR == other.MAJOR && MINOR < other.MINOR ) return true;
		if (MAJOR == other.MAJOR && MINOR == other.MINOR && REVISION < other.REVISION) return true;
		return false;
		
	}

};