#include "dom.hpp"
#include <iostream>

NodeBase::NodeBase(TypeEnum type_enum) : type_enum(type_enum) {}
NodeBase::~NodeBase() {}

void NodeBase::add_child(NodeBase *child) { children.push_back(child); }

string NodeBase::to_string() const { return ""; }

// BFS to print the DOM tree
void NodeBase::print(NodeBase *node) {
  if (NULL == node) {
    cerr << "Given node is empty" << endl;
    return;
  }

  int level = -1;
  tuple<NodeBase *, int> head(node, 0);
  vector<tuple<NodeBase *, int>> frontier = {head};

  while (!frontier.empty()) {
    tuple<NodeBase *, int> removed = frontier.front();
    NodeBase *removed_nb = get<0>(removed);

    frontier.erase(frontier.begin());

    string res = removed_nb->to_string();

    if (level < get<1>(removed)) {
      res = "\n" + res;
      level = get<1>(removed);
    }
    cout << res << "  ";

    if (!(removed_nb->children.empty())) {
      for (NodeBase *nb : removed_nb->children) {
        tuple<NodeBase *, int> tuple(nb, level + 1);
        frontier.push_back(tuple);
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

string TextNode::to_string() const { return "{TEXT, data: " + type_data + "}"; }

ElementNode::ElementNode(const string &tag) : NodeBase(ELEMENT), tag(tag) {}
ElementNode::ElementNode(const string &tag, const AttributeMap &attributes)
    : NodeBase(ELEMENT), tag(tag), attributes(attributes) {}
ElementNode::~ElementNode() {}

string ElementNode::to_string() const {
  string attrs = "";
  for (pair<string, string> p : attributes) {
    attrs += "(" + p.first + ";" + p.second + ")" + ",";
  }
  return "{ELEMENT, tag: " + tag + ", " + "attributes: [" + attrs + "]" + "}";
}