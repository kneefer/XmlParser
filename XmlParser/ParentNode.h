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

		void insert(shared_ptr<Node> nodeToInsert);
		void removeChild(shared_ptr<Node> nodeToRemove);

		~ParentNode() { }

		bool getIsParent() override { return true; }
	};

	inline void ParentNode::insert(shared_ptr<Node> nodeToInsert) {
		nodeToInsert->setParent(shared_ptr<ParentNode>(this));
		_nodes.push_back(nodeToInsert);
	}

	inline void ParentNode::removeChild(shared_ptr<Node> nodeToRemove) {
		if (!nodeToRemove->getIsParent()) {
			_nodes.remove(nodeToRemove);
			return;
		}

		auto parentNode = dynamic_pointer_cast<ParentNode>(nodeToRemove);
		for(auto& child : *parentNode) {
			parentNode->removeChild(child);
		}
	}
}