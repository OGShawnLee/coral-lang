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

    Typing get_typing(std::string name);

    void append(std::string name, const Typing &type, enum Entity entity);

    bool is_undefined(std::string name);
    bool is_duplicate(std::string name);
};

class Checker {
  std::shared_ptr<Scope> global_scope;
  bool failed;

  Typing check_binary_expression(
    const BinaryExpression *element,
    std::shared_ptr<Scope> &current_scope
  );

  Typing check_expression(
    const std::unique_ptr<Statement> &element,
    std::shared_ptr<Scope> &current_scope
  );
  Typing check_expression(
    const std::unique_ptr<Expression> &element,
    std::shared_ptr<Scope> &current_scope
  );
  Typing check_expression(
    const Expression *element,
    std::shared_ptr<Scope> &current_scope
  );

  void check_statement(
    const std::unique_ptr<Statement> &element, 
    std::shared_ptr<Scope> &current_scope
  );

  public:
    Checker(const Statement &element);
};