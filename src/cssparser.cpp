#include "css.hpp"
#include <iostream>
#include <sstream>

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

static void fill_simple_selector(const string &tag, char curr, SimpleSelector &selector)
{
  switch (curr)
  {
  case '\0':
    if (selector.tag.has_value())
    {
      throw runtime_error("fill_simple_selector error");
    }

    selector.tag.emplace(tag);
    break;

  case '.':
    if (!selector.class_selector.has_value())
    {
      selector.class_selector.emplace();
    }
    selector.class_selector.value().push_back(tag);
    break;

  case '#':
    if (selector.id_selector.has_value())
    {
      throw runtime_error("fill_simple_selector error1");
    }
    selector.id_selector.emplace(tag);
    break;

  default:
    break;
  }
}

static SimpleSelector analyze_element(const string &tag)
{
  SimpleSelector selector(std::nullopt, std::nullopt, std::nullopt);
  string name = "";
  char curr = '\0';
  for (int i = 0; i < tag.size(); ++i)
  {
    // Universal selector
    if ('*' == tag[i])
    {
      return SimpleSelector("*", std::nullopt, std::nullopt);
    }
  }
  throw runtime_error("TODO");
}

static void analyze_selector(const string &selector, Rule &rule)
{
  istringstream iss(selector);
  string element;
  while (std::getline(iss, element, ','))
  {
    // Trim leading and trailing whitespaces
    auto start = element.find_first_not_of(" \n");
    auto end = element.find_last_not_of(" \n");

    if (start != string::npos && end != string::npos)
    {
      string tag = element.substr(start, end - start + 1);

      // If an element contains a whitespace, error
      if (tag.find_first_of(" \n") != std::string::npos)
      {
        throw runtime_error("Invalid selector1");
      }
      SimpleSelector selector = analyze_element(tag);
      rule.selectors.push_back(selector);
    }
    else
    {
      throw runtime_error("Invalid selector2");
    }
  }
}

static void read_selector(Stylesheet &stylesheet, string &selector,
                          const string &input, int &pos)
{
  Rule rule;

  // Analyze selector
  analyze_selector(selector, rule);

  // If '{' not found, invalid css
  if (input.size() == pos)
  {
    throw runtime_error("Invalid css expression2");
  }

  // Try to find '}' and build selector
  while (pos < input.size() && input[pos] != '}')
  {
  }

  if (input.size() == pos)
  {
    throw runtime_error("Invalid css expression3");
  }
}

Stylesheet css_parse(const string &input)
{
  Stylesheet stylesheet;
  string selector = "";
  int pos = 0;
  while (pos < input.size())
  {
    if (input[pos] == '}')
    {
      throw runtime_error("Invalid css selector");
    }
    else if (input[pos] == '{')
    {
      if (!selector.empty())
      {
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
