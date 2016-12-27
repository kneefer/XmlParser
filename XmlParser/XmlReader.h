#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <sstream> 

#define TOKEN_TAG_OPEN '<'
#define TOKEN_TAG_CLOSE '>'
#define TOKEN_QUOTE '"'
#define TOKEN_ASSIGNMENT '='

using namespace std;

using sys_clock = chrono::high_resolution_clock;
using date_time = chrono::time_point<sys_clock>;

namespace Xml {
	enum State {
		S_Outside,
		S_TagName,
		S_Attribute,
		S_TagContent,
		S_ClosingException
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

		string getName() const { return _name; }
		string getValue() const { return _value; }
	};

	class Node {
		string _name;
		vector<Attribute> _attributes;
		shared_ptr<Node> _parent;
	public:
		Node(string name, vector<Attribute> const &attributes) {
			_name = name;
			_attributes = attributes;
		}
		virtual ~Node() = 0;

		string getName() const { return _name; }
		void setParent(shared_ptr<Node> parentToSet) { _parent = parentToSet; }
		shared_ptr<Node> getParent() const { return _parent; }
		vector<Attribute>& getAttributes() { return _attributes; }
	};

	inline Node::~Node() { }

	class LeafNode : public Node {
		string _innerText;
	public:
		LeafNode(string name, vector<Attribute> const &attributes, string innerText)
			: Node(name, attributes) {
			_innerText = innerText;
		}

		~LeafNode() { }

		string getInnerText() const { return _innerText; }
	};

	class ParentNode : public Node {
		vector<shared_ptr<Node>> _childrenNodes;
	public:
		ParentNode(string name, vector<Attribute> const &attributes, vector<shared_ptr<Node>>& childNodes)
			: Node(name, attributes) {
			_childrenNodes = childNodes;
		}

		~ParentNode() { }

		vector<shared_ptr<Node>>& _getNodes() { return _childrenNodes; }
	};

	class XmlReader {
		ifstream fileStreamInputFile;
		vector<shared_ptr<Node>> _nodes;
		char _currentChar;

		vector<shared_ptr<Node>> parse() {

			vector<shared_ptr<Node>> toReturn;

			string currentTagName;
			vector<Attribute> currentAttributes;
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
					while (processStateAttribute(state, currentAttributes));
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

		void processStateOutside(State& state) {

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

		void processStateTagName(State& state, string& currentTagName) {

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

		bool processStateAttribute(State& state, vector<Attribute>& attributes) {

			string attributeName;
			string attributeValue;

			skipSpaces();

			if (_currentChar == TOKEN_TAG_CLOSE) {
				state = S_TagContent;
				return false;
			}

			if (isalpha(_currentChar)) {
				attributeName += _currentChar;
			} else {
				throw "Expected attribute name starting with alpha char. Given: " + _currentChar;
			}

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

		static void setParentForChildren(vector<shared_ptr<Node>> childrenNodes, shared_ptr<ParentNode> parentNodeToSet) {
			for (auto& childrenNode : childrenNodes) {
				childrenNode->setParent(parentNodeToSet);
			}
		}

		shared_ptr<Node> processStateTagContent(State& state, string const &tagName, vector<Attribute> const &attributes) {

			string plainContent;

			skipSpaces();
			if (_currentChar == TOKEN_TAG_OPEN) // Embedded tags or closing tag 
			{
				fileStreamInputFile.unget();
				vector<shared_ptr<Node>> children = parse();
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

		auto elapsed() {
			static date_time prev;
			auto now = sys_clock::now();
			auto result = chrono::duration_cast<chrono::milliseconds>(now - prev).count();
			prev = now;
			return result;
		}

	public:
		XmlReader(): _currentChar(0)
		{
			_nodes = vector<shared_ptr<Node>>();
		}

		vector<shared_ptr<Node>>& getNodes() { return _nodes; }

		bool load(string strFileName) {
			if (!ifstream(strFileName)) {
				throw "Not found '" + strFileName + "' file.";
			}

			fileStreamInputFile.open(strFileName, ios::in);
			return fileStreamInputFile.is_open();
		}

		double run() {

			elapsed();
			_nodes = parse();
			return elapsed() / 1000.0;
		}

		bool close() {
			if (fileStreamInputFile.is_open()) {
				fileStreamInputFile.close();
				return true;
			}

			return false;
		}
	};
}