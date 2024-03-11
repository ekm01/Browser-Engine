#include "css.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

Value::~Value() {}
string Value::to_string() const { return ""; }

float Value::to_px() const { return 0.0; }

Keyword::Keyword(string &keyword) : keyword(keyword) {}
Keyword::~Keyword(){};
string Keyword::to_string() const { return "keyword: " + keyword; }

Length::Length(float length) : length(length) {}
Length::~Length() {}
string Length::to_string() const {
  return "length: " + std::to_string(length) + "px";
}

float Length::to_px() const { return length; }

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
    : r(r), g(g), b(b), alpha(alpha) {}
Color::~Color() {}
string Color::to_string() const {
  return "color: [" + std::to_string(r) + ", " + std::to_string(g) + ", " +
         std::to_string(b) + ", " + std::to_string(alpha) + "]";
}

SimpleSelector::SimpleSelector(const optional<string> &tag,
                               const optional<string> &id_selector,
                               const optional<vector<string>> &class_selector)
    : tag(tag), id_selector(id_selector), class_selector(class_selector) {}
SimpleSelector::~SimpleSelector() {}

string SimpleSelector::to_string() const {
  string res = "{";
  if (tag.has_value()) {
    res += "tag: " + tag.value() + "; ";
  }

  if (id_selector.has_value()) {
    res += "id_selector: " + id_selector.value() + "; ";
  }

  if (class_selector.has_value()) {
    res += "class_selector: [";
    for (string cs : class_selector.value()) {
      res += cs + ", ";
    }
    res += "]; ";
  }
  res += "}";
  return res;
}

string dec_to_string(Declaration &dec) {
  return "{name: " + dec.name + "value: " + dec.value.to_string() + "}";
}

string rule_to_string(Rule &rule) {
  string res = "{";
  res += "selectors: [";

  for (SimpleSelector sel : rule.selectors) {
    res += sel.to_string() + ", ";
  }
  res += "]; ";

  res += "declarations: [";
  for (Declaration dec : rule.declarations) {
    res += dec_to_string(dec) + ", ";
  }
  res += "]";
  res += "}";
  return res;
}

static void fill_simple_selector(const string &tag, char curr,
                                 SimpleSelector &selector) {
  switch (curr) {
  case '\0':
    if (selector.tag.has_value()) {
      throw runtime_error("Tag is already set");
    }

    selector.tag.emplace(tag);
    break;

  case '.':
    if (!selector.class_selector.has_value()) {
      selector.class_selector.emplace();
    }
    selector.class_selector.value().push_back(tag);
    break;

  case '#':
    if (selector.id_selector.has_value()) {
      throw runtime_error("Id selector is already set");
    }
    selector.id_selector.emplace(tag);
    break;

  default:
    break;
  }
}

static SimpleSelector analyze_element(const string &tag) {
  if (tag.empty()) {
    throw runtime_error("Tag cannot be empty");
  }

  SimpleSelector selector(std::nullopt, std::nullopt, std::nullopt);
  string name = "";
  char curr = '\0';
  int pos = 0;

  if ('#' == tag[pos] || '.' == tag[pos]) {
    curr = tag[pos];
    pos++;
  }

  if (pos == tag.size()) {
    throw runtime_error("Selector must be followed by a character");
  }

  while (pos < tag.size()) {

    // Universal selector
    if ('*' == tag[pos]) {
      if (0 != pos || tag.size() - 1 != pos) {
        throw runtime_error("Universal selector must be single");
      }
      return SimpleSelector("*", std::nullopt, std::nullopt);

      // Id and class selectors
    } else if ('.' == tag[pos] || '#' == tag[pos]) {
      if (tag.size() - 1 == pos) {
        throw runtime_error("Selector must be followed by a character");
      }
      fill_simple_selector(name, curr, selector);
      curr = tag[pos];
      name = "";

      // Tag name
    } else {
      name += tag[pos];
    }
    pos++;
  }
  fill_simple_selector(name, curr, selector);
  return selector;
}

static void analyze_selector(const string &selector, Rule &rule) {
  istringstream iss(selector);
  string element;
  int contains_uniselector = 0;
  while (std::getline(iss, element, ',')) {

    // Trim leading and trailing whitespaces
    auto start = element.find_first_not_of(" \n");
    auto end = element.find_last_not_of(" \n");

    if (start != string::npos && end != string::npos) {
      string tag = element.substr(start, end - start + 1);

      // If an element contains a whitespace, error
      if (tag.find_first_of(" \n") != std::string::npos) {
        throw runtime_error("Element contains a whitespace");
      }
      SimpleSelector selector = analyze_element(tag);
      if (selector.tag.has_value() && selector.tag.value() == "*") {
        contains_uniselector = 1;
      }
      rule.selectors.push_back(selector);
    } else {
      throw runtime_error("Element is empty");
    }
  }
  if (contains_uniselector == 1 && rule.selectors.size() != 1) {
    throw runtime_error(
        "Universal selector should not be given with other selectors");
  }
}

static void read_selector(Stylesheet &stylesheet, string &selector,
                          const string &input, int &pos) {
  Rule rule;

  // Analyze selector
  analyze_selector(selector, rule);

  // If '{' not found, invalid css
  if (input.size() == pos) {
    throw runtime_error("Invalid css expression2");
  }

  // Try to find '}' and build selector
  while (pos < input.size() && input[pos] != '}') {
  }

  if (input.size() == pos) {
    throw runtime_error("Invalid css expression3");
  }
}

Stylesheet css_parse(const string &input) {
  Stylesheet stylesheet;
  string selector = "";
  int pos = 0;
  while (pos < input.size()) {
    if (input[pos] == '}') {
      throw runtime_error("Invalid css selector");
    } else if (input[pos] == '{') {
      if (!selector.empty()) {
        read_selector(stylesheet, selector, input, pos);
        selector = "";
      }
    }
    selector += input[pos];
    pos++;
  }

  return stylesheet;
}

int main() {
  Rule rule;
  analyze_selector(" h1, \n\n h2.avda, \n #div.gfs.fda, .gfsgf.gfsga#h5 \n",
                   rule);
  cout << rule_to_string(rule) << endl;
  return 0;
}
