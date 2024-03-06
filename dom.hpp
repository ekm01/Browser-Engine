#ifndef DOM_HPP
#define DOM_HPP

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum TypeEnum { ELEMENT, TEXT };

class NodeBase {
  
public:
  vector<NodeBase *> children;
  TypeEnum type_enum;

  NodeBase(TypeEnum type_enum);
  virtual ~NodeBase();

  void add_child(NodeBase *child);

  virtual string to_string() const;

  static void print(NodeBase *node);
};

class TextNode : public NodeBase {
public:
  string type_data;

  TextNode(const string &text);

  string to_string() const override;
};

typedef unordered_map<string, string> AttributeMap;

class ElementNode : public NodeBase {
public:
  string tag;
  AttributeMap attributes;

  ElementNode(const string &tag);
  ElementNode(const string &tag, const AttributeMap &attributes);
  ~ElementNode();

  string to_string() const override;
};

#endif // !DOM_HPP
