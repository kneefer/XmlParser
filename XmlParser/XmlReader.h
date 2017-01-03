#pragma once

#include "Common.h"

#include "Node.h"
#include "ParentNode.h"
#include "LeafNode.h"
#include "XmlContainer.h"

using namespace std;

using sys_clock = chrono::high_resolution_clock;
using date_time = chrono::time_point<sys_clock>;

namespace Xml {
	typedef list<shared_ptr<Node>>::iterator iterator;
	typedef list<shared_ptr<Node>>::const_iterator const_iterator;

	class XmlReader : public XmlContainer {
		ifstream fileStreamInputFile;
		char _currentChar;

		list<shared_ptr<Node>> parse();

		void processStateOutside(State& state);
		void processStateTagName(State& state, string& currentTagName);
		bool processStateAttribute(State& state, map<string, string>& attributes);
		shared_ptr<Node> processStateTagContent(State& state, string const& tagName, map<string, string> const& attributes);

		inline void skipSpaces(bool getAtTheBeginning = false);
		inline static void setParentForChildren(list<shared_ptr<Node>> childrenNodes, shared_ptr<ParentNode> parentNodeToSet);
		string generateXmlString(ostringstream& os, XmlContainer* nodes, int nestLvl) const;
		
		auto elapsed() {
			static date_time prev;
			auto now = sys_clock::now();
			auto result = chrono::duration_cast<chrono::milliseconds>(now - prev).count();
			prev = now;
			return result;
		}

		inline bool openXmlFile(string fileName);
		inline bool closeXmlFile();
	public:
		XmlReader();
		
		double runParsing(string fileName);
		string generateXmlString();
	};

	inline XmlReader::XmlReader() : _currentChar(0) {
		
	}
}
