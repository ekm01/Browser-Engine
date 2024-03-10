#include "css.hpp"
#include <iostream>

Value::~Value() {}

float Value::to_px() const { return 0.0; }

Keyword::Keyword(string &keyword) : keyword(keyword) {}
Keyword::~Keyword(){};

Length::Length(float length) : length(length) {}
Length::~Length() {}

float Length::to_px() const { return length; }

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
    : r(r), g(g), b(b), alpha(alpha) {}
Color::~Color() {}

SimpleSelector::SimpleSelector(const optional<string> &tag,
                               const optional<string> &id_selector,
                               const optional<vector<string>> &class_selector)
    : tag(tag), id_selector(id_selector), class_selector(class_selector) {}
SimpleSelector::~SimpleSelector() {}

static string analyze_selector(const string &selector) { return ""; }

static void read_selector(Stylesheet &stylesheet, string &selector,
                          const string &input, int &pos) {
  // Analyze selector
  selector = analyze_selector(selector);

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

// int main() { return 0; }
