#pragma once

#include "Utils.h"
#include "Parser.cpp"
#include "Statement.cpp"

class Transpiler {
  std::string output;

  std::string handle_arr_literal(const Expression* literal);
  std::string handle_str_literal(const Expression* literal);
  std::string handle_literal(const Expression* literal);
    
  std::string handle_expression(
    const std::unique_ptr<Statement> &statement,
    const size_t &indentation = 0
  );
  std::string handle_expression(
    const std::unique_ptr<Expression> &expression,
    const size_t &indentation = 0
  );
  std::string handle_expression(
    const Expression *expression, 
    const size_t &indentation = 0
  );

  void handle_statement(
    const std::unique_ptr<Statement> &statement,
    const size_t &indentation = 0
  );
  void handle_loop_statement(
    const std::unique_ptr<Statement> &statement,
    const size_t indentation = 0
  );

  public: 
    void transpile(const std::string &file_path, const std::string &output_path);
};