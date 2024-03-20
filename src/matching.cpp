#include "matching.hpp"
#include "cssparser.hpp"
#include <iostream>

MatchedNode::MatchedNode(NodeBase *dom_node, const PropertyMap &values)
    : dom_node(dom_node), values(values) {}
MatchedNode::~MatchedNode() {}

string MatchedNode::to_string() const {
  string result =
      "{dom_node: " + this->dom_node->to_string() + ", property map: [";

  for (pair<string, Value> p : this->values) {
    result += "(" + p.first + ";" + p.second.to_string() + ")" + ",";
  }

  result += "]}";
  return result;
}

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
  NodeBase *dom = html_parse("examples/html/test.html");
  Stylesheet css = css_parse("examples/css/test.css");
  NodeBase::print(dom);
  cout << "\n\n" << endl;
  cout << stylesheet_to_string(css) << endl;

  NodeBase::free_node(dom);
  free_values(css);
  return 0;
}
