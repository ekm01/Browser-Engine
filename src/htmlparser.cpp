#include "dom.hpp"
#include <fstream>
#include <iostream>

extern string trim_spaces(const string &untrimmed, const string &message);

static int only_space_or_newline(string &text) {
  for (char c : text) {
    if (!isspace(c)) {
      return 0;
    }
  }
  return 1;
}

static string collapse_spaces(string &text) {
  string result = "";
  int previous = 0;

  for (char c : text) {
    if (c == ' ') {
      if (!previous) {
        result += c;
        previous = 1;
      }
    } else {
      result += c;
      previous = 0;
    }
  }

  return result;
}

static int is_valid_value(const string &value) {
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

static pair<string, string> get_key_value(string &statement) {
  size_t pos = statement.find('=');

  // '=' is found
  if (pos != string::npos) {
    string key =
        trim_spaces(statement.substr(0, pos), "Unexpected attribute key");

    string value =
        trim_spaces(statement.substr(pos + 1), "Unexpected attribute value");

    // Check if value is ok
    if (!is_valid_value(value)) {
      throw runtime_error("Unexpected attribute value");
    }
    value = value.substr(1, value.size() - 2);
    return make_pair(key, value);
  } else {
    throw runtime_error("Unexpected attribute assignment");
  }
}

static ElementNode *read_element(ifstream &input, char c) {

  // Skip '<'
  c = input.get();

  if (isspace(c)) {
    throw runtime_error("No space allowed after <");
  }

  string tag = "";

  // Try to find the tag

  while (input) {
    if ('>' == c) {
      // Only found a single tag without any attributes
      if (tag.find('/') == 0) {
        tag = tag.substr(1);
      }
      return new ElementNode(tag);
    }

    if (isspace(c)) {
      // Found tag so break to get attributes
      c = input.get();
      break;
    }
    tag += c;
    c = input.get();
  }

  if (input.eof()) {
    throw runtime_error("Invalid html element");
  }

  if (tag.find('/') == 0) {
    tag = tag.substr(1);
  }
  cout << tag << endl;

  // Try to find attributes
  AttributeMap map;
  string attr = "";
  int quotect = 0;
  while (input) {
    if ('>' == c) {
      return new ElementNode(tag, map);
    } else if ('"' == c) {
      quotect++;
    }

    attr += c;

    if (2 == quotect) {
      pair<string, string> key_value = get_key_value(attr);
      map.insert(key_value);
      attr = "";
      quotect = 0;
    }
    c = input.get();
  }
  throw runtime_error("Invalid html element");
}

static NodeBase *parse_aux(ifstream &input, ElementNode *root, string &text,
                           vector<string> &tags) {
  char c;
  while (input && (c = input.get()) != '<') {
    if (c != '\n') {
      text += c;
    }
  }

  if (!text.empty() && !only_space_or_newline(text)) {
    root->add_child(new TextNode(text));
  }
  text = "";

  ElementNode *node = read_element(input, c);

  if (node->tag == root->tag) {
    NodeBase::free_node(node);
    tags.pop_back();
    return root;
  }
  tags.push_back(node->tag);

  ElementNode *child = (ElementNode *)parse_aux(input, node, text, tags);
  root->add_child(child);

  if (tags.size() != 0) {
    return parse_aux(input, root, text, tags);
  }
  return root;
}

NodeBase *html_parse(const string &input) {
  ifstream file;
  file.open(input);
  if (!file.is_open()) {
    throw runtime_error("Error opening the file");
  }

  ElementNode root("");
  string text = "";
  vector<string> tags;
  NodeBase *temp_result = parse_aux(file, &root, text, tags);

  file.close();
  return temp_result->children[0];
}

/*int main(void) {
  string filename = "examples/html/test1.html";
  NodeBase *res = html_parse(filename);
  NodeBase::print(res);
  NodeBase::free_node(res);
  return 0;
}*/
