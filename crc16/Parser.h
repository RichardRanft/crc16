#pragma once
#include <map>
#include <list>
#include <utility>
#include <string>

enum EArgType
{
	FLAG,
	VALUE
}; 

class CParser
{
private:
	std::map<std::string, std::pair<EArgType, bool>> m_argRequired;
	std::map<std::string, std::string> m_argDictionary;
	std::string m_state;

public:
	CParser();
	CParser(std::map<std::string, std::pair<EArgType, bool>>);
	~CParser();

	bool Parse(std::list<std::string>);
	std::map<std::string, std::string> GetOptions();
};
