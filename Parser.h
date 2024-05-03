#pragma once

#include <memory>
#include "Lexer.cpp"
#include "Statement.cpp"

class Parser {
  public:
    static Statement parse(const std::string &file_path);
};