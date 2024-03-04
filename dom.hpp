#ifndef DOM_HPP
#define DOM_HPP

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

typedef unordered_map<string, string> AttributeMap;

struct Element {
  string tag;
  AttributeMap attributes;
};

enum TypeEnum { ELEMENT, TEXT };

#endif
