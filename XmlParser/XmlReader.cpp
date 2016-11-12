#include "XmlReader.h"

XmlReader::XmlReader()
{
	m_bDocOpen = false;
	m_strParentNode = "";
	m_nLineNum = 0;
}

bool XmlReader::load(std::string strFileName)
{
	if (!exists(strFileName)) {
		Error::ThrowError2(ErrorCode::FileExistError);
		return false;
	} else {
		m_ifsInFile.open(strFileName, std::ios::in);
	}
	m_bDocOpen = isOpen();
	if (!m_bDocOpen) {
		Error::ThrowError2(ErrorCode::StreamError);
		return false;
	}
	return true;
}

void XmlReader::close()
{
	m_ifsInFile.close();
}

bool XmlReader::exists(std::string strFileName)
{
	std::ifstream ifsCheckFile(strFileName);
	return !!ifsCheckFile;
}

bool XmlReader::isOpen()
{
	if (!m_ifsInFile.is_open()) {
		return false;
	}
	return true;
}

XmlReader::node XmlReader::getElementByTagName(std::string strTagName)
{
	XmlReader::node node;
	XmlReader::pNode parentNode;
	XmlReader::cNode childNode;
	std::string strInnerText;
	std::string strAttributeName;
	std::string strAttributeValue;
	std::size_t sz = strTagName.size();
	std::string strInLine;
	std::string strTmp;
	std::string strOpenTag;
	int posLt = 0;
	int posGt = 0;
	int posFAtr = 0;
	int posLAtr = 0;
	int posLastAtr = 0;
	m_strParentNode = strTagName;

	while (std::getline(m_ifsInFile, strInLine, '\n')) {
		parentNode.strAttributeName._Pop_back_n(parentNode.strAttributeName.size());
		parentNode.strAttributeValue._Pop_back_n(parentNode.strAttributeValue.size());
		m_nLineNum = m_ifsInFile.tellg();
		if (strInLine != "") {

			if (strInLine[0] == '\t') {
				std::vector<int> tabPos;
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

							getAttributes(strOpenTag, strAttributeName, strAttributeValue);
							parentNode.strAttributeName.push_back(strAttributeName);
							parentNode.strAttributeValue.push_back(strAttributeValue);

						}
					}
					getInnerText(strInLine, strInnerText);
					parentNode.strInnerText = strInnerText;

				} else {
					childNode = getChildNodes();
					parentNode.childNode.push_back(childNode);
				}

				node.parentNode.push_back(parentNode);
			}
		}
	}
	return node;
}

XmlReader::cNode XmlReader::getChildNodes()
{
	XmlReader::cNode tmpNode;
	XmlReader::cNode childNode;
	std::string strInLine;
	std::string strInnerText;
	std::string strAttributeName;
	std::string strAttributeValue;
	std::string strTagName;
	std::string strEndTag;
	std::string strOpenTag;
	std::size_t sz;
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
		std::getline(m_ifsInFile, strInLine, '\n');
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

			m_lsstrChildNodeName.push(strTagName);
			posF = strInLine.find_last_of("<");
			posL = strInLine.find_last_of(">");

			if (strInLine.substr(posF + 1, posL - posF - 1) != "/" + m_lsstrChildNodeName.top()) {
				childNode.strAttributeName._Pop_back_n(childNode.strAttributeName.size());
				childNode.strAttributeValue._Pop_back_n(childNode.strAttributeValue.size());

				sz = strTagName.size();

				if (strInLine.substr(1, sz) == strTagName) {
					posLt = strInLine.find_first_of("<");
					posGt = strInLine.find_first_of(">");

					strOpenTag = strInLine.substr(posLt + 1, posGt - posLt - 1);

					if (strOpenTag.size() > sz) {
						while (strOpenTag.size() > sz) {
							getAttributes(strOpenTag, strAttributeName, strAttributeValue);
							childNode.strAttributeName.push_back(strAttributeName);
							childNode.strAttributeValue.push_back(strAttributeValue);
						}
					}

					posLt = strInLine.find_last_of("<");
					posGt = strInLine.find_last_of(">");

					strEndTag = strInLine.substr(posLt + 1, posGt - posLt - 1);

					if (strEndTag == "/" + strTagName) {
						getInnerText(strInLine, strInnerText);
						childNode.strInnerText = strInnerText;
					} else {
						tmpNode = getChildNodes();
						childNode.childNode.push_back(tmpNode);
					}
				} else {
					m_lsstrChildNodeName.pop();
					endTag = true;
				}
			} else {
				getInnerText(strInLine, strInnerText);
				childNode.strInnerText = strInnerText;
				m_lsstrChildNodeName.pop();
				endTag = true;
			}
		}
	}
	return childNode;
}

void XmlReader::getAttributes(std::string& strOpenTag, std::string& strAttName, std::string& strAttValue)
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

void XmlReader::getInnerText(std::string& strInLine, std::string& strInnerText)
{
	int posLt = 0;
	int posGt = 0;

	posLt = strInLine.find_last_of("<");
	posGt = strInLine.find_first_of(">");

	strInnerText = strInLine.substr(posGt + 1, posLt - posGt - 1);
}

std::ifstream& XmlReader::goToLine(std::ifstream& file, int lineNum)
{
	file.seekg(std::ios::beg);
	file.seekg(lineNum);

	return file;
}

bool XmlReader::pNode::hasChildNode()
{
	if (XmlReader::pNode::childNode.size() != 0) {
		return true;
	}
	return false;
}

bool XmlReader::cNode::hasChildNode()
{
	if (XmlReader::cNode::childNode.size() != 0) {
		return true;
	}
	return false;
}