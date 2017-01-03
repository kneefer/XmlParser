#include "XmlReader.h"

namespace Xml {

	list<shared_ptr<Node>> XmlReader::parse() {
		list<shared_ptr<Node>> toReturn;

		string currentTagName;
		map<string, string> currentAttributes;
		State state = State::S_Outside;

		while (fileStreamInputFile.get(_currentChar)) {
			switch (state) {
			case S_Outside:
				processStateOutside(state);
				break;
			case S_TagName:
				processStateTagName(state, currentTagName);
				break;
			case S_Attribute:
				while (processStateAttribute(state, currentAttributes)) { }
				break;
			case S_ClosingException:
				return toReturn;
			case S_TagContent:
				shared_ptr<Node> node = processStateTagContent(state, currentTagName, currentAttributes);
				currentAttributes.clear();
				currentTagName.clear();
				toReturn.push_back(node);
				break;
			}
		}

		return toReturn;
	}

	void XmlReader::processStateOutside(State& state) {
		if (_currentChar != TOKEN_TAG_OPEN)
			return;

		char tmpChar;
		if (fileStreamInputFile.get(tmpChar)) {
			if (tmpChar == '/') {
				state = S_ClosingException;
				fileStreamInputFile.unget();
				return;
			}

			fileStreamInputFile.unget();
		}
		state = S_TagName;
	}

	void XmlReader::processStateTagName(State& state, string& currentTagName) {
		if (!(isalpha(_currentChar) || _currentChar == '_' || _currentChar == ':'))
			throw "Expected correct first character of tag name. Given: " + _currentChar;

		do {
			if (isspace(_currentChar)) {
				state = S_Attribute;
				break;
			}

			if (_currentChar == TOKEN_TAG_CLOSE) {
				state = S_TagContent;
				break;
			}

			currentTagName += _currentChar;
		} while (fileStreamInputFile.get(_currentChar));
	}

	bool XmlReader::processStateAttribute(State& state, map<string, string>& attributes) {
		string attributeName;
		string attributeValue;

		skipSpaces();

		if (_currentChar == TOKEN_TAG_CLOSE) {
			state = S_TagContent;
			return false;
		}

		if (isalpha(_currentChar))
			attributeName += _currentChar;
		else 
			throw "Expected attribute name starting with alpha char. Given: " + _currentChar;

		while (fileStreamInputFile.get(_currentChar)) {
			if (!isalnum(_currentChar))
				break;
			attributeName += _currentChar;
		}

		skipSpaces();

		if (_currentChar != TOKEN_ASSIGNMENT)
			throw "Expected equal sign after attribute name. Given: " + _currentChar;

		skipSpaces(true);

		if (_currentChar != TOKEN_QUOTE)
			throw "Expected quotation sign to start attribute value. Given: " + _currentChar;

		while (fileStreamInputFile.get(_currentChar)) {
			if (_currentChar == TOKEN_QUOTE)
				break;
			attributeValue += _currentChar;
		}

		attributes.emplace(attributeName, attributeValue);

		// To have _currentChar populated during next execution of this function
		fileStreamInputFile.get(_currentChar);

		return true;
	}

	void XmlReader::skipSpaces(bool getAtTheBeginning) {
		if (getAtTheBeginning)
			fileStreamInputFile.get(_currentChar);

		do {
			if (!isspace(_currentChar))
				break;
		} while (fileStreamInputFile.get(_currentChar));
	}

	void XmlReader::setParentForChildren(list<shared_ptr<Node>> childrenNodes, shared_ptr<ParentNode> parentNodeToSet) {
		for (auto& childrenNode : childrenNodes)
			childrenNode->setParent(parentNodeToSet);
	}

	shared_ptr<Node> XmlReader::processStateTagContent(State& state, string const& tagName, map<string, string> const& attributes) {
		string plainContent;

		skipSpaces();
		if (_currentChar == TOKEN_TAG_OPEN) // Embedded tags or closing tag 
		{
			fileStreamInputFile.unget();
			list<shared_ptr<Node>> children = parse();
			fileStreamInputFile.unget();

			stringbuf buffer;
			fileStreamInputFile.get(buffer, TOKEN_TAG_CLOSE);
			if (buffer.str() != ("/" + tagName))
				throw "Tag's content not properly enclosed";
			state = State::S_Outside;

			auto parent = make_shared<ParentNode>(tagName, attributes, children);
			setParentForChildren(children, parent);
			return parent;
		}
		else // Plain string content
		{
			do {
				if (_currentChar == TOKEN_TAG_OPEN) { // Closing tag
					stringbuf buffer;
					fileStreamInputFile.get(buffer, TOKEN_TAG_CLOSE);
					size_t found = buffer.str().find("/" + tagName);
					if (found == string::npos) // Not found
						throw "Tag's content not properly enclosed";
					state = State::S_Outside;
					break;
				}

				plainContent += _currentChar;
			} while (fileStreamInputFile.get(_currentChar));

			return static_pointer_cast<Node>(make_shared<LeafNode>(tagName, attributes, plainContent));
		}
	}

	bool XmlReader::openXmlFile(string fileName) {
		if (!ifstream(fileName))
			throw "Not found '" + fileName + "' file.";

		fileStreamInputFile.open(fileName, ios::in);
		return fileStreamInputFile.is_open();
	}

	double XmlReader::runParsing(string fileName) {
		if (!openXmlFile(fileName))
			throw "Any file to parse is not open";

		elapsed();
		_nodes = parse();
		closeXmlFile();
		return elapsed() / 1000.0;
	}

	bool XmlReader::closeXmlFile() {
		if (fileStreamInputFile.is_open()) {
			fileStreamInputFile.close();
			return true;
		}
		return false;
	}

	string XmlReader::generateXmlString() {
		ostringstream os;
		return generateXmlString(os, shared_ptr<XmlContainer>(this), 0);
	}

	string XmlReader::generateXmlString(ostringstream& os, shared_ptr<XmlContainer> nodes, int nestLvl) const {
		for(auto& node : *nodes.get()) {
			os << string(nestLvl, '\t') << TOKEN_TAG_OPEN << node->getName();
			for(auto& attr : node->getAttributes()) {
				os << " " << attr.first << TOKEN_ASSIGNMENT << TOKEN_QUOTE << attr.second << TOKEN_QUOTE;
			}
			os << TOKEN_TAG_CLOSE;
			if(node->getIsParent()) {
				os << endl;
				auto parentCasted = dynamic_pointer_cast<ParentNode>(node);
				generateXmlString(os, parentCasted, nestLvl + 1);
				os << endl << string(nestLvl, '\t');
			} else { // Is leaf
				auto leafCasted = dynamic_pointer_cast<LeafNode>(node);
				os << leafCasted->getInnerText();
			}

			os << TOKEN_TAG_OPEN << '/' << node->getName() << TOKEN_TAG_CLOSE << endl;
		}

		return nestLvl == 0
			? os.str()
			: "";
	}
}
