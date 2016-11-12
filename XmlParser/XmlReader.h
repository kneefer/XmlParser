#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include "windows.h"
#include "Error.h"
#include "atlstr.h"

using namespace std;

namespace Xml {

	class XmlReader : private Error 
	{
	public:
		struct cNode {
			vector<cNode> childNodes;
			string strInnerText;
			vector<string> strAttributeNames;
			vector<string> strAttributeValues;
			bool haschildNodes();
		};
		struct pNode {
			vector<cNode> childNodes;
			string strInnerText;
			vector<string> strAttributeNames;
			vector<string> strAttributeValues;
			bool haschildNodes();
		};

		struct node {
			vector<pNode> parentNode;
			string strInnerText;
			vector<string> strAttributeNames;
			vector<string> strAttributeValues;
		};

		XmlReader();
		bool load(string strFileName);
		void close();
		XmlReader::node getElementByTagName(string strTagName);

	private:
		bool exists(string strFileName);
		bool isOpen();
		XmlReader::cNode getchildNodess();
		void getAttributes(string& strOpenTag, string& strAttName, string& strAttValue);
		void getInnerText(string& strInLine, string& strInnerText);

		ifstream& goToLine(ifstream& file, int lineNUm);

		ifstream   m_ifsInFile;
		string     m_strParentNode;
		size_t     m_nLineNum;
		bool            m_bDocOpen;

		stack<string> m_lsstrchildNodesName;
	};

}