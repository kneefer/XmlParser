#pragma once

#include "Common.h"
#include "Node.h"
#include "Attribute.h"

using namespace std;

namespace Xml
{
	class ParentNode : public Node {
		vector<shared_ptr<Node>> _childrenNodes;
	public:
		ParentNode(string name, vector<Attribute> const &attributes, vector<shared_ptr<Node>>& childNodes)
			: Node(name, attributes) {
			_childrenNodes = childNodes;
		}

		~ParentNode() { }

		bool getIsParent() override { return true; }
		vector<shared_ptr<Node>>& getNodes() { return _childrenNodes; }
	};
}