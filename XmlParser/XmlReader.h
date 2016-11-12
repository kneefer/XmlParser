#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include "windows.h"
#include "Error.h"
#include "atlstr.h"

class XmlReader : private Error 
{
public:
	struct cNode {
		std::vector<cNode> childNode;
		std::string strInnerText;
		std::vector<std::string> strAttributeName;
		std::vector<std::string> strAttributeValue;
		bool hasChildNode();
	};
	struct pNode {
		std::vector<cNode> childNode;
		std::string strInnerText;
		std::vector<std::string> strAttributeName;
		std::vector<std::string> strAttributeValue;
		bool hasChildNode();
	};

	struct node {
		std::vector<pNode> parentNode;
		std::string strInnerText;
		std::vector<std::string> strAttributeName;
		std::vector<std::string> strAttributeValue;
	};

	XmlReader();
	bool load(std::string strFileName);
	void close();
	XmlReader::node getElementByTagName(std::string strTagName);

private:
	bool exists(std::string strFileName);
	bool isOpen();
	XmlReader::cNode getChildNodes();
	void getAttributes(std::string& strOpenTag, std::string& strAttName, std::string& strAttValue);
	void getInnerText(std::string& strInLine, std::string& strInnerText);

	std::ifstream& goToLine(std::ifstream& file, int lineNUm);

	std::ifstream   m_ifsInFile;
	std::string     m_strParentNode;
	std::size_t     m_nLineNum;
	bool            m_bDocOpen;

	std::stack<std::string> m_lsstrChildNodeName;
};