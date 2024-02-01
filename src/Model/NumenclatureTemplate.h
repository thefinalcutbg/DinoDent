#pragma once
#include <array>
#include <vector>
#include <string>


// THIS ONE WORKS, BUT INTELLISENSE CAN'T PARSE IT RIGHT:

template<std::size_t N>
struct CompileTimeString
{
	char data_[N];
};

template<std::size_t N>
CompileTimeString(const char(&)[N]) -> CompileTimeString<N>;

template<CompileTimeString ... strings>
class Numenclature
{
	constexpr static std::array names = { strings.data_ ... }; //ensures consistency

    size_t m_idx{ 0 };
public:

	Numenclature() {}
    Numenclature(size_t index) { setIndex(index); }

    bool setIndex(size_t idx) {

        if (idx >= names.size()) return false;

		m_idx = idx;

		return true;
	}

	int getIndex() const { return m_idx; }

	const char* getName() const { return names[m_idx]; };

	static const std::array<const char*, names.size()>& getNames() { return names; }


	std::vector<std::string> getNamesAsStrings() const {

		std::vector<std::string> result;

		for (auto& name : names) result.push_back(name);

		return result;
	}

};



template<size_t size, std::array<const char*, size>& static_obj, int defaultIndex = 0>
class NumStatic {

	inline const static std::array<const char*, static_obj.size()>& s_names = static_obj;
	
    int m_idx{ defaultIndex < 0 || defaultIndex>=size ? 0 : defaultIndex };

public:

	NumStatic() {}

    NumStatic(int idx) { setIndex(idx); };

    bool setIndex(int idx) {

        if (idx >= s_names.size()) return false;

		m_idx = idx;

		return true;
	}

	int getIndex() const { return m_idx; }

	std::string getName() const { return s_names[m_idx]; };

	static const std::array<const char*, size>& getNames() { return s_names; };

	std::vector<std::string> getNamesAsStrings() const {

		std::vector<std::string> result;

		for (auto& name : s_names) result.push_back(name);
		
		return result;
	}
};

template<std::vector<std::string_view>& dyn_obj, int defaultIndex = 0>
class NumDynamic
{
	inline const static std::vector<std::string_view>& s_names = dyn_obj;

	int m_idx{ defaultIndex < 0 || defaultIndex >= dyn_obj.size() ? 0 : defaultIndex };

public:

	NumDynamic() {}

	NumDynamic(int idx) { setIndex(idx); };

	bool setIndex(int idx) {

		if (idx < 0 || idx >= s_names.size()) return false;

		m_idx = idx;

		return true;
	}

	int getIndex() const { return m_idx; }

	std::string_view getName() const { return s_names[m_idx]; };

	const std::vector<std::string_view>& getNamesAsStrings() { return s_names; }
};


/*
 
//Example:

std::array<const char*, 3> meds{ "Hello ", "Generic ", "World" };
typedef Numenclature<meds.size(), meds> TestNum;

typedef Numenclature<meds.size(), meds, 2> TestNum2;

*/
