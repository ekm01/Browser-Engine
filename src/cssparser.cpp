#include "css.hpp"
#include <iostream>

Value::~Value() {}

float Value::to_px() const { return 0.0; }

Keyword::Keyword(string &keyword) : keyword(keyword) {}
Keyword::~Keyword(){};

Length::Length(float length) : length(length) {}
Length::~Length() {}

float Length::to_px() const { return length; }

Color::Color(uint8_t r,
             uint8_t g,
             uint8_t b,
             uint8_t alpha) : r(r), g(g), b(b), alpha(alpha) {}
Color::~Color() {}

SimpleSelector::SimpleSelector(const optional<string> &tag = nullopt, const optional<string> &id_selector = nullopt, const optional<vector<string>> &class_selector = nullopt) : tag(tag), id_selector(id_selector), class_selector(class_selector) {}
SimpleSelector::~SimpleSelector() {}

int main()
{

    return 0;
}
