#include "XmlReader.h"

using namespace Xml;

int main() {

	XmlReader xmlReader;

	if (xmlReader.load("Test.xml")) {

		cout << "Duration: " << xmlReader.run() << " seconds" << endl;
		xmlReader.close();
		auto nodes = xmlReader.getNodes();

		cin.get();
	}
}