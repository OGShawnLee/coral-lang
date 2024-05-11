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
    
    std::shared_ptr<Scope> parent;
    std::map<std::string, Typing> entities;
    bool failed;

    static std::shared_ptr<Scope> create(std::shared_ptr<Scope> &parent);

    void append(std::string name, const Typing &type, enum Entity entity);

    bool is_duplicate(std::string name); 
};

class Checker {
  std::shared_ptr<Scope> global_scope;
  bool failed;

  void check_expression(const std::unique_ptr<Statement> &element);
  void check_statement(
    const std::unique_ptr<Statement> &element, 
    std::shared_ptr<Scope> &current_scope
  );

  public:
    Checker(const Statement &element);
};