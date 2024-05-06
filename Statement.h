#pragma once

#include <vector>
#include <memory>
#include "Lexer.h"

class Statement {
  public:
    enum class Kind {
      PROGRAM,
      STATEMENT,
      EXPRESSION,
    };

    enum class Type {
      STATEMENT,
      VARIABLE_DECLARATION,
      CONSTANT_DECLARATION,
      STRUCT_DECLARATION,
      FUNCTION_DECLARATION,
      ENUM_DECLARATION,
      IF_STATEMENT,
      ELSE_STATEMENT,
      LOOP_STATEMENT,
    };

    Type type;
    Kind kind;
    std::vector<std::unique_ptr<Statement>> children;

    Statement();

    virtual void print(size_t indent = 0) const;
};