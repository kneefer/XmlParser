#pragma once

#include "Common.h"
#include "Node.h"
#include "Attribute.h"

using namespace std;

namespace Xml
{
	class LeafNode : public Node {
		string _innerText;
	public:
		LeafNode(string name, map<string, string> const &attributes, string innerText)
			: Node(name, attributes) {
			_innerText = innerText;
		}

		LeafNode(string name, map<string, string> const &attributes)
			: Node(name, attributes) {
			_innerText = "";
		}

		~LeafNode() { }

		bool getIsParent() override { return false; }
		string getInnerText() const { return _innerText; }
	};
}