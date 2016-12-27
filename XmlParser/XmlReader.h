#pragma once

#include "Common.h"

#include "Node.h"
#include "ParentNode.h"
#include "LeafNode.h"

using namespace std;

using sys_clock = chrono::high_resolution_clock;
using date_time = chrono::time_point<sys_clock>;

namespace Xml {
	class XmlReader {
		ifstream fileStreamInputFile;
		vector<shared_ptr<Node>> _nodes;
		char _currentChar;

		vector<shared_ptr<Node>> parse();

		void processStateOutside(State& state);
		void processStateTagName(State& state, string& currentTagName);
		bool processStateAttribute(State& state, vector<Attribute>& attributes);
		shared_ptr<Node> processStateTagContent(State& state, string const& tagName, vector<Attribute> const& attributes);

		void skipSpaces(bool getAtTheBeginning = false);
		static void setParentForChildren(vector<shared_ptr<Node>> childrenNodes, shared_ptr<ParentNode> parentNodeToSet);
		auto elapsed() {
			static date_time prev;
			auto now = sys_clock::now();
			auto result = chrono::duration_cast<chrono::milliseconds>(now - prev).count();
			prev = now;
			return result;
		}

	public:
		XmlReader();

		vector<shared_ptr<Node>>& getNodes();
		bool load(string strFileName);
		double run();
		bool close();
		void print();
	};

	inline XmlReader::XmlReader() : _currentChar(0) {
		_nodes = vector<shared_ptr<Node>>();
	}

	inline vector<shared_ptr<Node>>& XmlReader::getNodes() {
		return _nodes;
	}
}
