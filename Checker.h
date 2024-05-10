#pragma once

#include <map>
#include "Variable.h"
#include "Function.h"
#include "Struct.h"

class Scope {
  public:
    enum Entity {
      CONSTANT,
      FUNCTION,
      VARIABLE,
      STRUCT,
    };

    std::map<std::string, Typing> entities;
    bool failed;

    Scope();

    void append(std::string name, const Typing &type, enum Entity entity);

    bool is_duplicate(std::string name); 
};

class Checker {
  Scope global_scope;
  bool failed;

  void check_expression(const std::unique_ptr<Statement> &element);
  void check_statement(
    const std::unique_ptr<Statement> &element, 
    Scope &scope
  );

  public:
    Checker(const Statement &element);
};