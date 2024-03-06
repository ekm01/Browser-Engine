#include "dom.hpp"
#include <iostream>

int is_valid_value(const string &value) {
  // Check if value has at least three chars
  if (value.size() < 3) {
    return 0;
  }

  // Check if value starts and ends with double quotes
  if (value.front() != '"' || value.back() != '"') {
    return 0;
  }

  // Check if substring inside double quotes is not empty
  string inside_quotes = value.substr(1, value.size() - 2);
  return !inside_quotes.empty();
}

pair<string, string> get_key_value(string &statement) {
  size_t pos = statement.find('=');

  // '=' is found
  if (pos != std::string::npos) {
    string key = statement.substr(0, pos);
    string value = statement.substr(pos + 1);

    // Check if value is ok
    if (!is_valid_value(value)) {
      throw runtime_error("Unexpected attribute value");
    }
    return make_pair(key, value);
  } else {
    throw runtime_error("Unexpected attribute assignment");
  }
}

ElementNode *read_element(string &input, int &pos) {
  if (input[pos] != '<') {
    throw runtime_error("Element has to start with a <");
  }

  pos++;

  if (isspace(input[pos])) {
    throw runtime_error("No space allowed after <");
  }

  string tag = "";

  // Try to find the tag
  while (pos < input.size()) {
    if ('>' == input[pos]) {
      // Only found a single tag without any attributes
      if (tag.find('/') == 0) {
        tag = tag.substr(1);
      }
      pos++;
      return new (malloc(sizeof(ElementNode))) ElementNode(tag);
    }

    if (isspace(input[pos])) {
      // Found tag so break to get attributes
      pos++;
      break;
    }
    tag += input[pos];
    pos++;
  }

  if (input.size() == pos) {
    throw runtime_error("Invalid html element");
  }

  if (tag.find('/') == 0) {
    tag = tag.substr(1);
  }

  // Try to find attributes
  AttributeMap map;
  string attr = "";
  while (pos < input.size()) {
    if ('>' == input[pos]) {
      if (!attr.empty()) {
        pair<string, string> key_value = get_key_value(attr);
        map.insert(key_value);
      }
      pos++;
      return new (malloc(sizeof(ElementNode))) ElementNode(tag, map);
    }

    if (!isspace(input[pos])) {
      attr += input[pos];
    } else {
      if (!attr.empty()) {
        pair<string, string> key_value = get_key_value(attr);
        map.insert(key_value);
        attr = "";
      }
    }
    pos++;
  }
  throw runtime_error("Invalid html element");
}

int only_space_or_newline(string &str) {
  for (char c : str) {
    if (!isspace(c)) {
      return 0;
    }
  }
  return 1;
}

NodeBase *parse(string &input, ElementNode *root, int &pos, string &text,
                vector<string> &tags) {
  while (pos < input.size() && input[pos] != '<') {
    text += input[pos];
    pos++;
  }

  if (!text.empty() && !only_space_or_newline(text)) {
    root->add_child(new (malloc(sizeof(TextNode))) TextNode(text));
  }
  text = "";

  ElementNode *node = read_element(input, pos);

  if (node->tag == root->tag) {
    node->~ElementNode();
    free(node);
    tags.pop_back();
    return root;
  }
  tags.push_back(node->tag);

  ElementNode *child = (ElementNode *)parse(input, node, pos, text, tags);
  root->add_child(child);

  if (tags.size() != 0) {
    return parse(input, root, pos, text, tags);
  }
  return root;
}

/*int main(void) {
  string value =
      "<html\n        >\n\n\n<body>                 \n      <h1>Title</h1><div "
      "id=\"main\"      test=\"test1\"       \n  \n\n  "
      "\nclass=\"test\"><p>Hello "
      "<em>world<test>344<a></a></test></em>!</p></div></body></html>";
  vector<string> tags;
  int pos = 0;
  ElementNode root("");
  string text = "";
  NodeBase *res = parse(value, &root, pos, text, tags);
  NodeBase::print(root.children[0]);
  return 0;
}*/
