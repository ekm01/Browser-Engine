#ifndef CSS_HPP
#define CSS_HPP

#include <optional>
#include <string>
#include <vector>

using namespace std;

class Value {
public:
  virtual ~Value();

  virtual float to_px() const;
  virtual string to_string() const;
};

class Keyword : public Value {
public:
  string keyword;

  Keyword(string &keyword);
  ~Keyword();

  string to_string() const override;
};

class Length : public Value {
public:
  float length;

  Length(float length);
  ~Length();

  float to_px() const override;
  string to_string() const override;
};

class Color : public Value {
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t alpha;

  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
  ~Color();
  string to_string() const override;
};

struct Declaration {
  string property;
  Value *value;
};

class SimpleSelector {

public:
  optional<string> tag;
  optional<string> id_selector;
  optional<vector<string>> class_selector;

  SimpleSelector(const optional<string> &tag,
                 const optional<string> &id_selector,
                 const optional<vector<string>> &class_selector);
  ~SimpleSelector();

  string to_string() const;

  unsigned int specificity() const;
};

struct Rule {
  vector<SimpleSelector> selectors;
  vector<Declaration> declarations;
};

struct Stylesheet {
  vector<Rule> rules;
};

Stylesheet css_parse(const string &input);

#endif // !CSS_HPP
