#pragma once

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <chrono>
#include <iterator>
#include <memory>
#include <sstream> 

#define TOKEN_TAG_OPEN '<'
#define TOKEN_TAG_CLOSE '>'
#define TOKEN_QUOTE '"'
#define TOKEN_ASSIGNMENT '='

namespace Xml
{
	enum State {
		S_Outside,
		S_TagName,
		S_Attribute,
		S_TagContent,
		S_ClosingException,
		S_SelfCloseException
	};
}