#include "dom.hpp"

template <typename T> class Node {
public:
  vector<Node *> children;
  TypeEnum type_enum;
  T type_data;

  Node(const string &text) : type_enum(TEXT), type_data(text) {}

  Node(const Element &element) : type_enum(ELEMENT), type_data(element) {}

  void add_child(Node *child) { this->children.push_back(child); }
};

int main(void) { return 0; }
