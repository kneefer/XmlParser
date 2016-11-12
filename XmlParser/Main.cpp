#include "XmlReader.h"

int main() {

	XmlReader xmlR;

	if (xmlR.load("path\\to\\xml\\doc")) {

		XmlReader::node xmlNode;
		xmlNode = xmlR.getElementByTagName("TagName");

		//std::cout << xmlNode.parentNode[0]
	}
}