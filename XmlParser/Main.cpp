#include "XmlReader.h"

using namespace Xml;

int main() {

	XmlReader xmlReader;

	if (xmlReader.load("Test.xml")) {

		auto nodes = xmlReader.getNodes();

		cin.get();
	}
}