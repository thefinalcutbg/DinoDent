#include "WhenToTake.h"

#include <set>

#include <JsonCpp/json.h>
#include "Resources.h"

std::vector<std::string> s_tagList;
std::set<int> s_hasOffset;

void WhenToTake::initialize()
{
	Json::Value jsonWhen;

	Json::Reader().parse(Resources::fromPath(":/json/json_when.json"), jsonWhen);

	s_tagList = { jsonWhen.size() + 1, std::string() };

	for (auto& obj : jsonWhen)
	{
		s_tagList[obj["key"].asInt()] = obj["value"].asString();

		if (obj["offset"].asBool()) s_hasOffset.insert(obj["key"].asInt());

	}
}

const std::vector<std::string>& WhenToTake::tagsList()
{
	return s_tagList;
}

bool WhenToTake::isValidValue(const std::string& valueStr)
{
	for (auto& v : s_tagList) {
		if (valueStr == v) return true;
	}
	return false;
}

bool WhenToTake::addTag(const std::string& valueStr)
{
	for (int i = 1; i < s_tagList.size(); i++)
	{
		if (valueStr == s_tagList[i])
		{
			m_tags.push_back(i);
			return true;
		}
	}

	return false;
}

bool WhenToTake::addTag(int key)
{
	if (key > 0 && key < s_tagList.size()) {
		m_tags.push_back(key);
		return true;
	}

	return false;
}

std::vector<std::string> WhenToTake::getTags() const
{
	std::vector<std::string> result;

	for (auto key : m_tags) result.push_back(s_tagList[key]);

	return result;
}

bool WhenToTake::removeTag(int idx)
{
	if (idx >= 0 && idx < m_tags.size())
	{
		m_tags.erase(m_tags.begin() + idx);
		return true;
	}

	return false;
}

void WhenToTake::setOffset(unsigned int offset)
{
	this->m_offset = offset;
}

bool WhenToTake::offsetAllowed() const
{
	for (auto& tag : m_tags)
	{
		if(s_hasOffset.count(tag)) return true;
	}

	return false;
}

bool WhenToTake::enableOffset(bool enabled)
{
	if (offsetAllowed()) {
		m_offsetEnabled = enabled;
		return true;
	}

	return false;
	
}

std::string WhenToTake::parse() const
{
	std::string result;

	if (m_tags.empty()) return result;

	if (offsetAllowed() && m_offsetEnabled)
	{
		
		result += std::to_string(m_offset);
		result += m_offset == 1 ? " минута " : " минути ";

	}

	for (auto& tag : m_tags)
	{
		result += s_tagList[tag] + " ";
	}

	return result;
}
