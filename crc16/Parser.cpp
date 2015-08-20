#include "Parser.h"

CParser::CParser()
{
	m_state = "";
}

CParser::CParser(std::map<std::string, std::pair<EArgType, bool>> argList)
{
	m_state = "";
	m_argRequired = argList;
	for (std::map<std::string, std::pair<EArgType, bool>>::const_iterator itr = m_argRequired.begin(); itr != m_argRequired.end(); itr++)
	{
		std::string empty = "";
		std::pair<std::string, std::string> pair = std::pair<std::string, std::string>((*itr).first, empty);
		m_argDictionary.insert(pair);
	}
}

CParser::~CParser()
{
}

bool CParser::Parse(std::list<std::string> args)
{
	for (std::list<std::string>::const_iterator itr = args.begin(); itr != args.end(); itr++)
	{
		std::map <std::string, std::string>::iterator it = m_argDictionary.find((*itr));
		if (it != m_argDictionary.end())
		{
			m_state = (*itr);
			if (m_argRequired[(*it).first].first == EArgType::FLAG)
			{
				m_argDictionary[m_state] = "true";
				m_state = "";
			}
		}
		else
		{
			m_argDictionary[m_state] = (*itr);
			m_state = "";
		}
	}
	for (std::map<std::string, std::string>::const_iterator itr = m_argDictionary.begin(); itr != m_argDictionary.end(); itr++)
	{
		if (m_argRequired[(*itr).first].second && m_argDictionary[(*itr).first].empty())
			return false;
	}
	return true;
}

std::map<std::string, std::string> CParser::GetOptions()
{
	return m_argDictionary;
}
