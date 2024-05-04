#pragma once

#include <vector>
#include <memory>
#include "Utils.h"
#include "Expression.h"
#include "Statement.h"

class Variable : public Statement {
  public:
    Type type = Type::VARIABLE_DECLARATION;
    Kind kind = Kind::STATEMENT;
    std::string name;
    std::unique_ptr<Expression> value;
    std::string typing;
    bool is_constant;
    bool is_field = false;

    static PeekPtr<Variable> build(Stream &stream, const size_t &start_index);

    static PeekPtr<Variable> build_as_field(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};