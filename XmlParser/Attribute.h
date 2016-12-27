#pragma once

#include "Common.h"

using namespace std;

namespace Xml
{
	struct Attribute {
	private:
		string _name;
		string _value;
	public:
		Attribute(string name, string value) {
			_name = name;
			_value = value;
		}

		string getName() const { return _name; }
		string getValue() const { return _value; }
	};
}