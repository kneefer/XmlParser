#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <mutex>
#include <memory>
#include <algorithm> 
#include "windows.h"
#include "Error.h"
#include "atlstr.h"

using namespace std;

namespace Xml {

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
		vector<shared_ptr<Node>> _nodes;
	public:
		XmlReader() {
			_nodes = vector<shared_ptr<Node>>();
		}

		bool load(string strFileName) {
			return true;
		}

		vector<shared_ptr<Node>>& getNodes() { return _nodes; }
	};
}