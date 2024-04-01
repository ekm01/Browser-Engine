#include "matching.hpp"
#include <iostream>
#include <string>

MatchedNode::MatchedNode(NodeBase *dom_node, PropertyMap &values)
    : dom_node(dom_node), values(values) {}
MatchedNode::MatchedNode() {}
MatchedNode::~MatchedNode() {}

string MatchedNode::to_string() const {
  string result =
      "{dom_node: " + this->dom_node->to_string() + ", property map: [";

  for (pair<string, Value *> p : this->values) {
    result += "(" + p.first + ";" + (*p.second).to_string() + ")" + ",";
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

static int match_element_selector(NodeBase *element, SimpleSelector &selector) {

  if (selector.tag.value_or(element->get_tag()) != element->get_tag()) {
    return 0;
  }

  string element_id = element->get_id().value_or("");
  if (selector.id_selector.value_or(element_id) != element_id) {
    return 0;
  }

  optional<ClassSet> element_classes = element->get_classes();
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

static vector<Rule> match_rule(NodeBase *element, Stylesheet &css) {
  vector<Rule> matched_rules = {};
  for (Rule rule : css.rules) {
    for (SimpleSelector selector : rule.selectors) {
      if (match_element_selector(element, selector)) {
        matched_rules.push_back(rule);
        break;
      }
    }
  }
  return matched_rules;
}

static PropertyMap create_map(NodeBase *element, Stylesheet &css) {
  PropertyMap map = {};
  vector<Rule> matched_rules = match_rule(element, css);

  int i = 0;
  while (matched_rules.size() != 0) {
    Rule rule = matched_rules.back();
    matched_rules.pop_back();
    for (Declaration dec : rule.declarations) {
      map.insert(make_pair(dec.property, dec.value));
    }
    ++i;
  }
  return map;
}

static void match_aux(NodeBase *dom, Stylesheet &css, MatchedNode *res) {
  if (nullptr == dom) {
    return;
  }

  for (int i = 0; i < dom->children.size(); ++i) {
    if (ELEMENT == dom->children[i]->type_enum) {
      MatchedNode *node = new MatchedNode();
      match_aux(dom->children[i], css, node);
      res->children.push_back(node);
    }
  }
  if (ELEMENT == dom->type_enum) {
    PropertyMap map = create_map(dom, css);
    res->dom_node = dom;
    res->values = map;
  }
}

MatchedNode *match(NodeBase *dom, Stylesheet &css) {
  MatchedNode *root = new MatchedNode();
  match_aux(dom, css, root);
  return root;
}

int main() {
  NodeBase *dom = html_parse("examples/html/test.html");
  Stylesheet css = css_parse("examples/css/test.css");
  NodeBase::print(dom);
  cout << "\n\n" << endl;
  cout << stylesheet_to_string(css) << endl;

  MatchedNode *res = match(dom, css);
  MatchedNode::print(res);
  free_values(css);
  NodeBase::free_node(dom);
  MatchedNode::free_node(res);
  return 0;
}
