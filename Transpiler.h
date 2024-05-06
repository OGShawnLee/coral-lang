#pragma once

#include "Utils.h"
#include "Parser.cpp"
#include "Statement.cpp"

class Transpiler {
  size_t indentation = 0;
  std::string output;

  std::string get_value(const std::unique_ptr<Statement> &statement);
  std::string get_value(const std::unique_ptr<Expression> &expression);
  std::string get_value(const Expression *expression);

  void handle_statement(const std::unique_ptr<Statement> &statement);

  public: 
    void transpile(const std::string &file_path, const std::string &output_path);
};