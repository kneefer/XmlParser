#include "XmlReader.h"

using namespace Xml;

int main() {

	XmlReader xmlR;

	if (xmlR.load("Test.xml")) {

		XmlReader::node xmlNode;
		xmlNode = xmlR.getElementByTagName("catalog");
		cin.get();
		//std::cout << xmlNode.parentNode[0]
	}
}