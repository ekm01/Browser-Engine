#ifndef DOM_HPP
#define DOM_HPP

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

enum TypeEnum { ELEMENT, TEXT };

class NodeBase {

public:
  vector<NodeBase *> children;
  TypeEnum type_enum;

  NodeBase(TypeEnum type_enum);
  virtual ~NodeBase();

  virtual string to_string() const;

  static void print(NodeBase *node);
  static void free_node(NodeBase *node);
};

class TextNode : public NodeBase {
public:
  string type_data;

  TextNode(const string &text);
  ~TextNode();

  string to_string() const override;
};

typedef unordered_map<string, string> AttributeMap;
typedef unordered_set<string> ClassSet;

class ElementNode : public NodeBase {
public:
  string tag;
  AttributeMap attributes;

  ElementNode(const string &tag);
  ElementNode(const string &tag, const AttributeMap &attributes);
  ~ElementNode();

  string to_string() const override;
  optional<ClassSet> get_classes() const;
  optional<string> get_id() const;
};

NodeBase *html_parse(const string &input);

#endif // !DOM_HPP
