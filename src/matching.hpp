#ifndef MATCHING_HPP
#define MATCHING_HPP

#include "cssparser.hpp"
#include "dom.hpp"

// Property-value map
typedef unordered_map<string, Value> PropertyMap;

class MatchedNode {
public:
  NodeBase *dom_node;
  PropertyMap values;
  vector<MatchedNode *> children;

  MatchedNode(NodeBase *dom_node, PropertyMap &values);
  ~MatchedNode();

  string to_string() const;
  static void print(MatchedNode *node);
  static void free_node(MatchedNode *node);
};

#endif // !MATCHING_HPP
