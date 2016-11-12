#include "XmlReader.h"

namespace Xml {

	XmlReader::XmlReader()
	{
		m_bDocOpen = false;
		m_strParentNode = "";
		m_nLineNum = 0;
	}

	bool XmlReader::load(string strFileName)
	{
		if (!exists(strFileName)) {
			Error::ThrowError(ErrorCode::FileExistError);
			return false;
		} else {
			m_ifsInFile.open(strFileName, ios::in);
		}
		m_bDocOpen = isOpen();
		if (!m_bDocOpen) {
			Error::ThrowError(ErrorCode::StreamError);
			return false;
		}
		return true;
	}

	void XmlReader::close()
	{
		m_ifsInFile.close();
	}

	bool XmlReader::exists(string strFileName)
	{
		ifstream ifsCheckFile(strFileName);
		return !!ifsCheckFile;
	}

	bool XmlReader::isOpen()
	{
		if (!m_ifsInFile.is_open()) {
			return false;
		}
		return true;
	}

	XmlReader::node XmlReader::getElementByTagName(string strTagName)
	{
		XmlReader::node node;
		XmlReader::pNode parentNode;
		XmlReader::cNode childNode;
		string strInnerText;
		string strAttributeNames;
		string strAttributeValues;
		size_t sz = strTagName.size();
		string strInLine;
		string strTmp;
		string strOpenTag;
		int posLt = 0;
		int posGt = 0;
		int posFAtr = 0;
		int posLAtr = 0;
		int posLastAtr = 0;
		m_strParentNode = strTagName;

		while (getline(m_ifsInFile, strInLine, '\n')) {
			parentNode.strAttributeNames._Pop_back_n(parentNode.strAttributeNames.size());
			parentNode.strAttributeValues._Pop_back_n(parentNode.strAttributeValues.size());
			m_nLineNum = m_ifsInFile.tellg();
			if (strInLine != "") {

				if (strInLine[0] == '\t') {
					vector<int> tabPos;
					for (int i = 0; i < strInLine.size(); i++) {
						if (strInLine[i] == '\t') {
							tabPos.push_back(i);
						}
					}
					for (int i = tabPos.size(); i > 0; i--) {
						strInLine.erase(tabPos[i - 1], 1);
						tabPos.pop_back();
					}
				}

				if (strInLine.substr(1, sz) == strTagName) {

					posLt = strInLine.find_last_of("<");
					posGt = strInLine.find_last_of(">");

					strTmp = strInLine.substr(posLt + 1, posGt - posLt - 1);

					if (strTmp == "/" + strTagName) {

						posLt = strInLine.find_first_of("<");
						posGt = strInLine.find_first_of(">");

						strOpenTag = strInLine.substr(posLt + 1, posGt - posLt - 1);

						if (strOpenTag.size() > sz) {

							while (strOpenTag.size() > sz) {

								getAttributes(strOpenTag, strAttributeNames, strAttributeValues);
								parentNode.strAttributeNames.push_back(strAttributeNames);
								parentNode.strAttributeValues.push_back(strAttributeValues);

							}
						}
						getInnerText(strInLine, strInnerText);
						parentNode.strInnerText = strInnerText;

					} else {
						childNode = getchildNodess();
						parentNode.childNodes.push_back(childNode);
					}

					node.parentNode.push_back(parentNode);
				}
			}
		}
		return node;
	}

	XmlReader::cNode XmlReader::getchildNodess()
	{
		XmlReader::cNode tmpNode;
		XmlReader::cNode childNode;
		string strInLine;
		string strInnerText;
		string strAttributeNames;
		string strAttributeValues;
		string strTagName;
		string strEndTag;
		string strOpenTag;
		size_t sz;
		int posF = 0;
		int posL = 0;
		int posS = 0;
		int posLt = 0;
		int posGt = 0;
		bool endTag = false;

		if (m_nLineNum != m_ifsInFile.tellg()) {
			goToLine(m_ifsInFile, m_nLineNum);
		}

		while (!endTag) {
			getline(m_ifsInFile, strInLine, '\n');
			m_nLineNum = m_ifsInFile.tellg();

			if (strInLine != "") {
				posF = strInLine.find_first_of("<");
				posL = strInLine.find_first_of(">");
				posS = strInLine.find_first_of(" ");

				if (posS < posL && posS > posF) {
					strTagName = strInLine.substr(posF + 1, posS - posF - 1);
				} else {
					strTagName = strInLine.substr(posF + 1, posL - posF - 1);
				}

				if (strTagName.substr(0, 1) == "/") {
					strTagName.erase(0, 1);
				}

				m_lsstrchildNodesName.push(strTagName);
				posF = strInLine.find_last_of("<");
				posL = strInLine.find_last_of(">");

				if (strInLine.substr(posF + 1, posL - posF - 1) != "/" + m_lsstrchildNodesName.top()) {
					childNode.strAttributeNames._Pop_back_n(childNode.strAttributeNames.size());
					childNode.strAttributeValues._Pop_back_n(childNode.strAttributeValues.size());

					sz = strTagName.size();

					if (strInLine.substr(1, sz) == strTagName) {
						posLt = strInLine.find_first_of("<");
						posGt = strInLine.find_first_of(">");

						strOpenTag = strInLine.substr(posLt + 1, posGt - posLt - 1);

						if (strOpenTag.size() > sz) {
							while (strOpenTag.size() > sz) {
								getAttributes(strOpenTag, strAttributeNames, strAttributeValues);
								childNode.strAttributeNames.push_back(strAttributeNames);
								childNode.strAttributeValues.push_back(strAttributeValues);
							}
						}

						posLt = strInLine.find_last_of("<");
						posGt = strInLine.find_last_of(">");

						strEndTag = strInLine.substr(posLt + 1, posGt - posLt - 1);

						if (strEndTag == "/" + strTagName) {
							getInnerText(strInLine, strInnerText);
							childNode.strInnerText = strInnerText;
						} else {
							tmpNode = getchildNodess();
							childNode.childNodes.push_back(tmpNode);
						}
					} else {
						m_lsstrchildNodesName.pop();
						endTag = true;
					}
				} else {
					getInnerText(strInLine, strInnerText);
					childNode.strInnerText = strInnerText;
					m_lsstrchildNodesName.pop();
					endTag = true;
				}
			}
		}
		return childNode;
	}

	void XmlReader::getAttributes(string& strOpenTag, string& strAttName, string& strAttValue)
	{
		int posF = 0;
		int posL = 0;

		posF = strOpenTag.find_first_of(" ");
		posL = strOpenTag.find_first_of("=");

		strAttName = strOpenTag.substr(posF + 1, posL - posF - 1);
		strOpenTag.erase(posF, posL - posF + 1);

		posF = strOpenTag.find_first_of("\"");
		posL = posF;
		posF = strOpenTag.find("\"", posF + 1);
		strAttValue = strOpenTag.substr(posL + 1, posF - posL - 1);
		strOpenTag.erase(posL, posF - posL + 1);
	}

	void XmlReader::getInnerText(string& strInLine, string& strInnerText)
	{
		int posLt = 0;
		int posGt = 0;

		posLt = strInLine.find_last_of("<");
		posGt = strInLine.find_first_of(">");

		strInnerText = strInLine.substr(posGt + 1, posLt - posGt - 1);
	}

	ifstream& XmlReader::goToLine(ifstream& file, int lineNum)
	{
		file.seekg(ios::beg);
		file.seekg(lineNum);

		return file;
	}

	bool XmlReader::pNode::haschildNodes()
	{
		if (XmlReader::pNode::childNodes.size() != 0) {
			return true;
		}
		return false;
	}

	bool XmlReader::cNode::haschildNodes()
	{
		if (XmlReader::cNode::childNodes.size() != 0) {
			return true;
		}
		return false;
	}
}