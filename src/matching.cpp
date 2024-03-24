#include "matching.hpp"
#include "dom.hpp"
#include <iostream>
#include <utility>

MatchedNode::MatchedNode(NodeBase *dom_node, PropertyMap &values)
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
  pair<MatchedNode *, int> head(node, 0);
  vector<pair<MatchedNode *, int>> frontier = {head};

  while (!frontier.empty()) {
    pair<MatchedNode *, int> removed = frontier.front();
    MatchedNode *removed_nb = removed.first;

    frontier.erase(frontier.begin());

    string res = removed_nb->to_string();

    if (level < removed.second) {
      res = "\n" + res;
      level = removed.second;
    }
    cout << res << "  ";

    if (!(removed_nb->children.empty())) {
      for (MatchedNode *nb : removed_nb->children) {
        pair<MatchedNode *, int> pair(nb, level + 1);
        frontier.push_back(pair);
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

static int match_element_selector(ElementNode &element,
                                  SimpleSelector &selector) {

  if (selector.tag.value_or(element.tag) != element.tag) {
    return 0;
  }

  string element_id = element.get_id().value_or("");
  if (selector.id_selector.value_or(element_id) != element_id) {
    return 0;
  }

  optional<ClassSet> element_classes = element.get_classes();
  if (selector.class_selector.has_value() && element_classes.has_value()) {
    vector<string> classes = selector.class_selector.value();
    ClassSet class_set = element_classes.value();

    for (string c : classes) {
      auto it = class_set.find(c);
      if (it == class_set.end()) {
        return 0;
      }
    }
  } else if (selector.class_selector.has_value() &&
             !element_classes.has_value()) {
    return 0;
  }

  return 1;
}

static MatchedNode *match_aux(NodeBase *dom, Stylesheet &css, MatchedNode *res,
                              PropertyMap &map) {
  if (nullptr == dom) {
    return res;
  }

  for (int i = 0; i < dom->children.size(); ++i) {
    if (ELEMENT == dom->type_enum) {

      MatchedNode *node = match_aux(
          dom->children[i], css, new MatchedNode(dom->children[i], map), map);
      res->children.push_back(node);
      map.clear();
    }
  }
  // TODO: Implement the logic
}

int main() {
  /*NodeBase *dom = html_parse("examples/html/test.html");
  Stylesheet css = css_parse("examples/css/test.css");
  NodeBase::print(dom);
  cout << "\n\n" << endl;
  cout << stylesheet_to_string(css) << endl;

  NodeBase::free_node(dom);
  free_values(css);*/
  AttributeMap map;
  map.insert(make_pair("id", "test"));
  map.insert(make_pair("class", " test1  test2 test3 test4 "));

  ElementNode teste("test", map);
  SimpleSelector tests({}, {},
                       vector<string>{"test1", "test2", "test3", "test4"});
  cout << match_element_selector(teste, tests) << endl;
  return 0;
}
