#pragma once

#include "Common.h"
#include "Node.h"

using namespace std;

namespace Xml {
	typedef list<shared_ptr<Node>>::iterator iterator;
	typedef list<shared_ptr<Node>>::const_iterator const_iterator;

	class XmlContainer {
	protected:
		list<shared_ptr<Node>> _nodes;

	public:
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		int size() const;
		shared_ptr<Node> nth(int index) const;
	};

	inline iterator XmlContainer::begin() {
		return _nodes.begin();
	}

	inline const_iterator XmlContainer::begin() const {
		return _nodes.begin();
	}

	inline iterator XmlContainer::end() {
		return _nodes.end();
	}

	inline const_iterator XmlContainer::end() const {
		return _nodes.end();
	}

	inline int XmlContainer::size() const {
		return _nodes.size();
	}

	inline shared_ptr<Node> XmlContainer::nth(int index) const {
		auto iter = this->begin();
		advance(iter, index);
		return *iter;
	}
}