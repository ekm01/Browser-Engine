#include "css.hpp"
#include <cctype>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

Value::~Value() {}
string Value::to_string() const { return ""; }

float Value::to_px() const { return 0.0; }

Keyword::Keyword(string &keyword) : keyword(keyword) {}
Keyword::~Keyword(){};
string Keyword::to_string() const { return keyword; }

Length::Length(float length) : length(length) {}
Length::~Length() {}
string Length::to_string() const { return std::to_string(length) + "px"; }

float Length::to_px() const { return length; }

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
    : r(r), g(g), b(b), alpha(alpha) {}
Color::~Color() {}
string Color::to_string() const {
  return "[" + std::to_string(r) + ", " + std::to_string(g) + ", " +
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

static string dec_to_string(Declaration &dec) {
  return "{" + dec.property + " : " + dec.value->to_string() + "}";
}

static string rule_to_string(Rule &rule) {
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

static string stylesheet_to_string(Stylesheet &stylesheet) {
  string res = "{";
  res += "selectors: [";
  for (int i = 0; i < stylesheet.rules.size(); ++i) {
    res += "Rule " + to_string(i) + " " + rule_to_string(stylesheet.rules[i]) +
           ", ";
  }
  res += "}";
  return res;
}

static void free_values(Stylesheet &stylesheet) {
  for (Rule rule : stylesheet.rules) {
    for (Declaration dec : rule.declarations) {
      delete dec.value;
    }
  }
}

static string trim_spaces(const string &untrimmed, const string &message) {
  auto start = untrimmed.find_first_not_of(" \n");
  auto end = untrimmed.find_last_not_of(" \n");

  if (start != string::npos && end != string::npos) {
    return untrimmed.substr(start, end - start + 1);
  } else {
    throw runtime_error(message);
  }
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
    string tag = trim_spaces(element, "Element is empty");

    // If an element contains a whitespace, error
    if (tag.find_first_of(" \n") != std::string::npos) {
      throw runtime_error("Element contains a whitespace");
    }
    SimpleSelector selector = analyze_element(tag);
    if (selector.tag.has_value() && selector.tag.value() == "*") {
      contains_uniselector = 1;
    }
    rule.selectors.push_back(selector);
  }
  if (contains_uniselector == 1 && rule.selectors.size() != 1) {
    throw runtime_error(
        "Universal selector should not be given with other selectors");
  }
}

static int contains_single_colon(const string &declaration) {
  size_t pos = declaration.find(':');
  if (pos != string::npos) {
    size_t second_pos = declaration.find(':', pos + 1);
    return second_pos == string::npos;
  }
  return 0;
}

static Color *analyze_color(const string &color) {
  if (color.size() != 7 || color.at(0) != '#') {
    throw runtime_error("Unexpected value for color");
  }
  int r = stoi(color.substr(1, 3), nullptr, 16);
  int g = stoi(color.substr(3, 5), nullptr, 16);
  int b = stoi(color.substr(5, color.size()), nullptr, 16);
  int alpha = 0;
  return new Color(r, g, b, alpha);
}

static int is_valid_pixel(const string &str) {

  // Regular expression pattern to match only numbers followed by "px"
  regex pattern("^\\d+px$");
  return regex_match(str, pattern);
}

static Value *analyze_value(const string &property, string &value) {
  if ("color" == property) {
    return analyze_color(value);
  } else if (is_valid_pixel(value)) {
    float length = stof(value.substr(0, value.size() - 2));
    return new Length(length);
  } else {
    return new Keyword(value);
  }
}

static Declaration analyze_declaration(string &declaration) {
  Declaration result;
  if (!contains_single_colon(declaration)) {
    throw runtime_error("Declaration contains more than one colon");
  }

  // Get property
  size_t colon_pos = declaration.find(':');
  string property = "";
  if (colon_pos != string::npos) {
    string property_untrimmed = declaration.substr(0, colon_pos);
    property = trim_spaces(property_untrimmed, "Property is empty");
    if (property.find_first_of(" \n") != std::string::npos) {
      throw runtime_error("Property contains a whitespace");
    }
    result.property = property;
  }

  // Get value
  string value_untrimmed =
      declaration.substr(colon_pos + 1, declaration.size());
  string value = trim_spaces(value_untrimmed, "Value is empty");
  if (value.find_first_of(" \n") != std::string::npos) {
    throw runtime_error("Value contains a whitespace");
  }
  result.value = analyze_value(property, value);

  return result;
}

static void analyze_declarations(string &declarations, Rule &rule) {
  string trimmed_dec = trim_spaces(declarations, "Declaration is empty");
  istringstream iss(trimmed_dec);
  string dec_str;
  while (std::getline(iss, dec_str, ';')) {
    string dec = trim_spaces(dec_str, "Declaration is empty");

    // If a declaration does not contain a colon, error
    if (dec.find_first_of(":") == string::npos) {
      throw runtime_error("Declaration does not contain a colon");
    }
    Declaration declaration = analyze_declaration(dec);
    rule.declarations.push_back(declaration);
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

  // Skip '{'
  pos++;

  string declarations = "";

  // Try to find '}' and build selector
  while (pos < input.size()) {
    if ('}' == input[pos]) {
      analyze_declarations(declarations, rule);
      break;
    } else {
      declarations += input[pos];
    }
    pos++;
  }

  pos++;

  // Skip newlines between selectors
  while (pos < input.size()) {
    if (!isspace(input[pos])) {
      break;
    }
    pos++;
  }
  stylesheet.rules.push_back(rule);
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
  string input = "h1 {\n"
                 "  text-align: center;\n"
                 "  color: #cc0000;\n"
                 "}\n"
                 "\n"
                 "h2 {\n"
                 "  text-align:\n center;\n"
                 "  color: #008000;\n"
                 "}\n"
                 "\n"
                 "p {\n"
                 "  text-align: center;\n"
                 "  color: #000080;\n"
                 "}\n"
                 "\n"
                 "h1, h2 , h3 { margin: auto; color: #cc0000; }\n"
                 "div.note { margin-bottom: 20px; padding: 10px; }\n"
                 "#answer { display: none; }\n"
                 "";
  Stylesheet css = css_parse(input);
  cout << stylesheet_to_string(css) << endl;
  free_values(css);
  return 0;
}
