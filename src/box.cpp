#include "box.hpp"
#include "matching.hpp"
#include <string>

BoxNode::BoxNode(BoxType boxtype) : type(boxtype) {
  this->area.border = {0.0, 0.0, 0.0, 0.0};
  this->area.margin = this->area.border;
  this->area.padding = this->area.border;
  this->area.content = {0.0, 0.0, 0.0, 0.0};
}
BoxNode::~BoxNode() {}

BoxNode *BoxNode::anonymous_boxing(Display child_type) {
  if (static_cast<int>(this->type) != static_cast<int>(child_type)) {
    for (BoxNode *node : this->children) {
      if (node->type == ANONYMOUS) {
        return node;
      }
    }
    BoxNode *anon = new BoxNode(ANONYMOUS);
    this->children.push_back(anon);
    return anon;
  }
  return this;
}

BoxNode *build_boxes(MatchedNode *matched_node) {
  BoxNode *root;
  switch (matched_node->get_display()) {
  case INLINE:
    root = new BoxNode(INLINE_BOX);
    root->matched_node = matched_node;
    break;
  case BLOCK:
    root = new BoxNode(BLOCK_BOX);
    root->matched_node = matched_node;
    break;
  default:
    terminate();
  }

  for (MatchedNode *child : matched_node->children) {
    Display display = child->get_display();
    if (display == INLINE || display == BLOCK) {
      root->anonymous_boxing(display)->children.push_back(build_boxes(child));
    }
  }
  return root;
}

int main() {
  NodeBase *dom = html_parse("examples/html/test.html");
  Stylesheet css = css_parse("examples/css/test.css");
  // NodeBase::print(dom);
  //  cout << "\n\n" << endl;
  //  cout << stylesheet_to_string(css) << endl;

  MatchedNode *res = match(dom, css);
  BoxNode *ress = build_boxes(res);
  MatchedNode::print(ress->matched_node);
  free_values(css);
  NodeBase::free_node(dom);
  MatchedNode::free_node(res);
  return 0;
}
