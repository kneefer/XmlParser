#pragma once

#include "Common.h"

using namespace std;

namespace Xml
{
	class Node {
		string _name;
		map<string, string> _attributes;
		shared_ptr<Node> _parent;
	public:
		Node(string name, map<string, string> const &attributes) {
			_name = name;
			_attributes = attributes;
		}
		virtual ~Node() = 0;
		virtual bool getIsParent() = 0;

		string getName() const { return _name; }
		void setParent(shared_ptr<Node> parentToSet) { _parent = parentToSet; }
		shared_ptr<Node> getParent() const { return _parent; }
		map<string, string>& getAttributes() { return _attributes; }
	};

	inline Node::~Node() { }
}