#ifndef BOX_HPP
#define BOX_HPP

#include "matching.hpp"
struct Edge {
  float right;
  float left;
  float top;
  float bottom;
};

struct Rectangle {
  float x;
  float y;
  float width;
  float height;
};

struct Area {
  Rectangle content;
  Edge margin;
  Edge border;
  Edge padding;
};

enum BoxType { BLOCK_BOX, INLINE_BOX, ANONYMOUS };

class BoxNode {
public:
  Area area;
  BoxType type;
  MatchedNode *matched_node;
  vector<BoxNode *> children;

  BoxNode();
  ~BoxNode();
};

#endif // !BOX_HPP
