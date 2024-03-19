#include "matching.hpp"
#include <iostream>

MatchedNode::MatchedNode(NodeBase *dom_node, const PropertyMap &values)
    : dom_node(dom_node), values(values) {}

string MatchedNode::to_string() const { throw runtime_error("TODO"); }

void MatchedNode::print(MatchedNode *node) {
  if (NULL == node) {
    return;
  }

  int level = -1;
  tuple<MatchedNode *, int> head(node, 0);
  vector<tuple<MatchedNode *, int>> frontier = {head};

  while (!frontier.empty()) {
    tuple<MatchedNode *, int> removed = frontier.front();
    MatchedNode *removed_nb = get<0>(removed);

    frontier.erase(frontier.begin());

    string res = removed_nb->to_string();

    if (level < get<1>(removed)) {
      res = "\n" + res;
      level = get<1>(removed);
    }
    cout << res << "  ";

    if (!(removed_nb->children.empty())) {
      for (MatchedNode *nb : removed_nb->children) {
        tuple<MatchedNode *, int> tuple(nb, level + 1);
        frontier.push_back(tuple);
      }
    }
  }
}

void MatchedNode::free_node(MatchedNode *node) {
  if (nullptr == node) {
    return;
  }

  for (int i = 0; i < node->children.size(); ++i) {
    free_node(node->children[i]);
  }
  delete node;
}

int main() {
  throw runtime_error("TODO");
  return 0;
}
