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
};

class Keyword : public Value {
public:
  string keyword;

  Keyword(string &keyword);
  ~Keyword();
};

class Length : public Value {
public:
  float length;

  Length(float length);
  ~Length();

  float to_px() const override;
};

class Color : public Value {
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t alpha;

  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);
  ~Color();
};

struct Declaration {
  string name;
  Value value;
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
};

struct Rule {
  vector<SimpleSelector> selectors;
  vector<Declaration> declarations;
};

struct Stylesheet {
  vector<Rule> rules;
};

#endif // !CSS_HPP
