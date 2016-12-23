#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <mutex>
#include <memory>
#include <regex>
#include <algorithm> 
#include "windows.h"
#include "atlstr.h"

#define TOKEN_TAG_OPEN '<'
#define TOKEN_TAG_CLOSE '>'
#define TOKEN_QUOTE '"'
#define TOKEN_ASSIGNMENT '='

using namespace std;

namespace Xml {
	enum State {
		S_Outside,
		S_TagName,
		S_Attribute,
		S_TagContent
	};

	struct Attribute {
	private:
		string _name;
		string _value;
	public:
		Attribute(string name, string value) {
			_name = name;
			_value = value;
		}

		string getName() { return _name; }
		string getValue() { return _value; }
	};

	class Node {
	private:
		string _name;
		vector<Attribute> _attributes;
	public:
		Node(string name, vector<Attribute>& attributes) {
			_name = name;
			_attributes = attributes;
		}
		virtual ~Node() = 0;

		string getName() { return _name; }
		vector<Attribute>& getAttributes() { return _attributes; }
	};

	class LeafNode : public Node {
	private:
		string _innerText;
	public:
		LeafNode(string name, vector<Attribute>& attributes, string innerText)
			: Node(name, attributes) {
			_innerText = innerText;
		}

		string getInnerText() { return _innerText; }
	};

	class ParentNode : public Node {
	private:
		vector<shared_ptr<Node>> _childNodes;
	public:
		ParentNode(string name, vector<Attribute>& attributes, vector<shared_ptr<Node>>& childNodes)
			: Node(name, attributes) {
			_childNodes = childNodes;
		}

		vector<shared_ptr<Node>>& _getNodes() { return _childNodes; }
	};

	class XmlReader {
	private:
		ifstream fileStreamInputFile;
		int filePosition;
		vector<shared_ptr<Node>> _nodes;

		char _currentChar;
		State _state;
	public:
		XmlReader() {
			_nodes = vector<shared_ptr<Node>>();
			_state = S_Outside;
		}

		vector<shared_ptr<Node>>& getNodes() { return _nodes; }

		bool load(string strFileName) {
			if (!ifstream(strFileName)) {
				throw "Not found '" + strFileName + "' file.";
				return false;
			}

			fileStreamInputFile.open(strFileName, ios::in);
			return fileStreamInputFile.is_open();
		}

		bool close() {
			if (fileStreamInputFile.is_open()) {
				fileStreamInputFile.close();
				return true;
			}

			return false;
		}

		vector<shared_ptr<Node>>& parse() {

			vector<shared_ptr<Node>> toReturn;

			string currentTagName;
			vector<Attribute> currentAttributes;

			while (fileStreamInputFile.get(_currentChar)) {
				switch (_state) {
				case S_Outside:    processStateOutside();
					break;
				case S_TagName:    processStateTagName(currentTagName);
					break;
				case S_Attribute:  while (processStateAttribute(currentAttributes));
					break;
				case S_TagContent: {
					auto node = processStateTagContent(currentTagName, currentAttributes);
					toReturn.push_back(node);
					break;
				} }
			}

			return toReturn;
		}

		void processStateOutside() {

			if (_currentChar != TOKEN_TAG_OPEN)
				return;

			_state = S_TagName;
		}

		void processStateTagName(string& currentTagName) {

			if (!isalpha(_currentChar))
				throw "Expected correct first character of tag name. Given: " + _currentChar;

			while (fileStreamInputFile.get(_currentChar)) {
				if (isalnum(_currentChar)) {
					currentTagName.append(&_currentChar);
					continue;
				}
				
				if (isspace(_currentChar)) {
					_state = S_Attribute;
				} else if (_currentChar == TOKEN_TAG_CLOSE) {
					_state = S_TagContent;
				} else {
					throw "Invalid character in tag name: " + _currentChar;
				}
				break;
			}
		}

		bool processStateAttribute(vector<Attribute>& attributes) {

			string attributeName;
			string attributeValue;

			skipSpaces();

			if (_currentChar == TOKEN_TAG_CLOSE) {
				_state = S_TagContent;
				return false;
			}

			if (isalpha(_currentChar)) {
				attributeName.append(&_currentChar);
			} else {
				throw "Expected attribute name starting with alpha char. Given: " + _currentChar;
				return false;
			}

			while (fileStreamInputFile.get(_currentChar)) {
				if (!isalnum(_currentChar))
					break;
				attributeName.append(&_currentChar);
			}

			skipSpaces();

			if (_currentChar != TOKEN_ASSIGNMENT) {
				throw "Expected equal sign after attribute name. Given: " + _currentChar;
				return false;
			}

			skipSpaces(true);

			if (_currentChar != TOKEN_QUOTE) {
				throw "Expected quotation sign to start attribute value. Given: " + _currentChar;
				return false;
			}

			while (fileStreamInputFile.get(_currentChar)) {
				if (_currentChar == TOKEN_QUOTE)
					break;
				attributeValue.append(&_currentChar);
			}

			attributes.push_back(Attribute(attributeName, attributeValue));

			// To have _currentChar populated during next execution of this function
			fileStreamInputFile.get(_currentChar); 

			return true;
		}

		void skipSpaces(bool getAtTheBeginning = false) {
			if (getAtTheBeginning)
				fileStreamInputFile.get(_currentChar);

			do {
				if (!isspace(_currentChar))
					break;
			} while (fileStreamInputFile.get(_currentChar));
		}

		shared_ptr<Node>& processStateTagContent(string& tagName, vector<Attribute>& attributes) {

		}
	};
}