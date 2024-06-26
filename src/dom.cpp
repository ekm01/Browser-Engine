#include "dom.hpp"
#include <iostream>
#include <sstream>

NodeBase::NodeBase(TypeEnum type_enum) : type_enum(type_enum) {}
NodeBase::~NodeBase() {}

string NodeBase::to_string() const { return ""; }

optional<ClassSet> NodeBase::get_classes() const { return {}; }

optional<string> NodeBase::get_id() const { return {}; }

string NodeBase::get_tag() const { return ""; }

// BFS to print the DOM tree
void NodeBase::print(NodeBase *node) {
  if (NULL == node) {
    return;
  }

  int level = -1;
  pair<NodeBase *, int> head(node, 0);
  vector<pair<NodeBase *, int>> frontier = {head};

  while (!frontier.empty()) {
    pair<NodeBase *, int> removed = frontier.front();
    NodeBase *removed_nb = removed.first;

    frontier.erase(frontier.begin());

    string res = removed_nb->to_string();

    if (level < removed.second) {
      res = "\n" + res;
      level = removed.second;
    }
    cout << res << "  ";

    if (!(removed_nb->children.empty())) {
      for (NodeBase *nb : removed_nb->children) {
        pair<NodeBase *, int> pair(nb, level + 1);
        frontier.push_back(pair);
      }
    }
  }
}

void NodeBase::free_node(NodeBase *node) {
  if (nullptr == node) {
    return;
  }

  for (int i = 0; i < node->children.size(); ++i) {
    free_node(node->children[i]);
  }
  delete node;
}

TextNode::TextNode(const string &text) : NodeBase(TEXT), type_data(text) {}
TextNode::~TextNode() {}

string TextNode::to_string() const {
  return "{TEXT, data: " + this->type_data + "}";
}

ElementNode::ElementNode(const string &tag) : NodeBase(ELEMENT), tag(tag) {}
ElementNode::ElementNode(const string &tag, const AttributeMap &attributes)
    : NodeBase(ELEMENT), tag(tag), attributes(attributes) {}
ElementNode::~ElementNode() {}

string ElementNode::to_string() const {
  string attrs = "";
  for (pair<string, string> p : this->attributes) {
    attrs += "(" + p.first + ";" + p.second + ")" + ",";
  }
  return "{ELEMENT, tag: " + this->tag + ", " + "attributes: [" + attrs + "]" +
         "}";
}

string ElementNode::get_tag() const { return this->tag; }

optional<string> ElementNode::get_id() const {
  auto it = this->attributes.find("id");
  if (this->attributes.end() == it) {
    return {};
  }
  return it->second;
}

optional<ClassSet> ElementNode::get_classes() const {
  auto it = this->attributes.find("class");

  // If there is no class, return None
  if (this->attributes.end() == it) {
    return {};
  }

  ClassSet class_set;
  istringstream iss(it->second);
  string class_name;
  int contains_uniselector = 0;
  while (std::getline(iss, class_name, ' ')) {
    class_set.insert(class_name);
  }
  return class_set;
}
