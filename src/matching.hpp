#ifndef MATCHING_HPP
#define MATCHING_HPP

#include "cssparser.hpp"
#include "dom.hpp"

// Property-value map
typedef unordered_map<string, Value *> PropertyMap;

class MatchedNode {
public:
  NodeBase *dom_node;
  PropertyMap values;
  vector<MatchedNode *> children;

  MatchedNode(NodeBase *dom_node, PropertyMap &values);
  MatchedNode();
  ~MatchedNode();

  string to_string() const;
  void set_values(PropertyMap &map);
  static void print(MatchedNode *node);
  static void free_node(MatchedNode *node);
};

#endif // !MATCHING_HPP
