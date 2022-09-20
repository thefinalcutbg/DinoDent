#pragma once
#include <vector>
#include <string>

class WhenToTake
{
	std::vector<int> m_tags;
	unsigned int m_offset{ 1 };
	bool m_offsetEnabled{ false };

public:
	static void initialize();

	static const std::vector<std::string>& tagsList();
	static bool isValidValue(const std::string& valueStr);

	bool addTag(const std::string& valueStr);
	bool addTag(int key);
	std::vector<std::string> getTags() const;
	bool hasValue() { return m_tags.size(); }
	bool removeTag(int idx);
	void setOffset(unsigned int offset);
	bool offsetAllowed() const;
	bool enableOffset(bool enabled);
	bool offsetEnabled() const { return m_offsetEnabled; }
	int getOffset() const { if (offsetAllowed() && offsetEnabled()) return m_offset; return 0; }
	const std::vector<int>& getTagIdx() const { return m_tags; }

	std::string parse() const;
};