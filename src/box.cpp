#include "box.hpp"
#include "matching.hpp"
#include <iostream>
#include <string>

BoxNode::BoxNode(BoxType boxtype) : type(boxtype) {
  Area area;
  Edge edge;
  Rectangle rect;
  rect.x = 0.0;
  rect.y = 0.0;
  rect.width = 0.0;
  rect.height = 0.0;
  edge.top = 0.0;
  edge.bottom = 0.0;
  edge.left = 0.0;
  edge.right = 0.0;
  area.border = edge;
  area.margin = edge;
  area.padding = edge;
  area.content = rect;
}
BoxNode::~BoxNode() {}

BoxNode *BoxNode::anonymous_boxing(Display child_type) {
  if (this->type != child_type) {
    if (!this->children.empty() && this->children.back()->type == ANONYMOUS) {
      return this->children.back();
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
