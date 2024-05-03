#pragma once

#include <vector>
#include <memory>
#include "Lexer.h"

class Statement {
  public:
    enum class Kind {
      PROGRAM,
      STATEMENT,
    };

    enum class Type {
      VARIABLE_DECLARATION,
      CONSTANT_DECLARATION,
    };

    Kind kind = Kind::STATEMENT;
    std::vector<std::unique_ptr<Statement>> children;

    virtual void print(size_t indent = 0) const;
};