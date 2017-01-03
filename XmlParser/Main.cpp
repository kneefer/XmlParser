#include "XmlReader.h"
#include <conio.h>
#include <algorithm>

using namespace Xml;

#define KEY_ESC_CODE         27
#define KEY_S_CODE           115
#define KEY_ARROW_UP_CODE    72
#define KEY_ARROW_DOWN_CODE  80
#define KEY_ARROW_LEFT_CODE  75
#define KEY_ARROW_RIGHT_CODE 77

XmlContainer nodes;
shared_ptr<Node> currentNode;
int currentIndex = 0;
unsigned char keyCode;
bool exitRequested = false;

void printNode(shared_ptr<Node> nodeToPrint, int currentIndex)
{
	string attributesToPrint;
	auto parent = nodeToPrint->getParent();
	bool isParent = nodeToPrint->getIsParent();
	bool firstLine = true;
	auto attributes = nodeToPrint->getAttributes();
	for(auto& attribute : attributes) {
		if (firstLine) {
			firstLine = false;
		} else {
			attributesToPrint.append("\t\t");
		}
		attributesToPrint.append(
			"K: " + attribute.first + 
			" V: " + attribute.second + "\n");
	}

	auto name = nodeToPrint->getName();
	auto parentName = parent ? parent->getName() : "NO_PARENT";
	cout << endl << "@@@@@ " << (isParent ? "PARENT" : "LEAF") << " NODE (Index #" << currentIndex << ") @@@@@" << endl;
	cout << "Element name:\t" << name << endl << "Parent name:\t" << parentName << endl;
	cout << "Attributes:\t" << ((attributes.size() > 0) ? attributesToPrint : "NO_ATTRIBUTES\n");

	if(isParent) {
		auto parentCasted = dynamic_pointer_cast<ParentNode>(nodeToPrint);
		cout << "Children count:\t" << parentCasted->size() << endl;
	} else { // Is leaf
		auto leafCasted = dynamic_pointer_cast<LeafNode>(nodeToPrint);
		cout << "Content:\t" << leafCasted->getInnerText() << endl;
	}

	cout << endl;
}

void saveParsedXml(XmlReader& xmlReader) {
	string outFileName = "out.xml";
	ofstream ofs;
	ofs.open(outFileName, ofstream::out, ofstream::trunc);
	if (!ofs.is_open())
		throw "Could not open file '" + outFileName + "' in output mode";

	ofs << xmlReader.generateXmlString();
	cout << "Output file '" << outFileName << "' generated";

	ofs.close();
}

void navigateGoPreviousElement() {
	if (currentIndex > 0) {
		currentIndex--;
	} else {
		cout << "You cannot go to the previous element" << endl;
		return;
	}
	auto parent = currentNode->getParent();
	auto parentCasted = dynamic_pointer_cast<ParentNode>(parent);
	if (parentCasted) {
		currentNode = parentCasted->nth(currentIndex);
	} else {
		currentNode = nodes.nth(currentIndex);
	}
}

void navigateGoNextElement() {
	auto parent = currentNode->getParent();
	auto parentCasted = dynamic_pointer_cast<ParentNode>(parent);
	if (parentCasted) {
		if(currentIndex >= (parentCasted->size() - 1)) {
			cout << "You cannot go to the next element" << endl;
			return;
		}
		currentNode = parentCasted->nth(++currentIndex);
	} else {
		if(currentIndex >= (nodes.size() - 1)) {
			cout << "You cannot go to the next element" << endl;
			return;
		}
		currentNode = nodes.nth(++currentIndex);
	}
}

void navigateStepOut() {
	auto parent = currentNode->getParent();
	
	if (parent) {
		auto parentOfParent = parent->getParent();
		if(parentOfParent) {
			auto parentOfParentCasted = dynamic_pointer_cast<ParentNode>(parentOfParent);

			auto vecIter = find(parentOfParentCasted->begin(), parentOfParentCasted->end(), parent);
			currentIndex = distance(parentOfParentCasted->begin(), vecIter);
		} else {
			currentIndex = 0;
		}
		
		currentNode = parent;
	} else {
		cout << "You cannot step out" << endl;
	}
}

void navigateStepInto() {
	if (currentNode->getIsParent()) {
		auto parentCasted = dynamic_pointer_cast<ParentNode>(currentNode);
		if (parentCasted->size() > 0) {
			currentNode = parentCasted->nth(0);
			currentIndex = 0;
			return;
		}
	}
	cout << "You cannot step into" << endl;
}

void main() {
	XmlReader xmlReader;

	if (!xmlReader.openXmlFile("Test.xml"))
		return;

	cout << "Duration: " << xmlReader.runParsing() << " seconds" << endl << endl;
	xmlReader.closeXmlFile();
	nodes = static_cast<XmlContainer>(xmlReader);

	if (nodes.size() < 1) {
		cout << "No nodes available";
		return;
	}

	currentNode = nodes.nth(0);

	cout << "- Navigate XML tree using arrow keys."
		<< endl << "- Press ESC to exit"
		<< endl << "- Press S to save XML" << endl;

	while(!exitRequested) {
		printNode(currentNode, currentIndex);

		keyCode = _getch();
		if (keyCode == 0 || keyCode == 0xE0) 
			keyCode = _getch();

		switch(keyCode) {
			case KEY_S_CODE:
				saveParsedXml(xmlReader);
				break;
			case KEY_ESC_CODE: 
				exitRequested = true;
				break;
			case KEY_ARROW_UP_CODE: 
				navigateGoPreviousElement();
				break;
			case KEY_ARROW_DOWN_CODE: 
				navigateGoNextElement();
				break;
			case KEY_ARROW_LEFT_CODE: 
				navigateStepOut();
				break;
			case KEY_ARROW_RIGHT_CODE: 
				navigateStepInto();
				break;
			default: 
				cout << "Unknown keycode: " << static_cast<int>(keyCode) << endl;
		}
	}

	cout << "\nPress enter to exit!";
	cin.get();
}
