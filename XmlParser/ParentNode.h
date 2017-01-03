#pragma once

#include "Common.h"
#include "Node.h"
#include "Attribute.h"
#include "XmlContainer.h"

using namespace std;

namespace Xml
{
	class ParentNode : public Node, public XmlContainer {
	public:
		ParentNode(string name, map<string, string> const &attributes, list<shared_ptr<Node>>& childNodes)
			: Node(name, attributes) {
			_nodes = childNodes;
		}

		~ParentNode() { }

		bool getIsParent() override { return true; }
	};
}