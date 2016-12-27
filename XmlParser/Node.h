#pragma once

#include "Common.h"
#include "Attribute.h"

using namespace std;

namespace Xml
{
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
}