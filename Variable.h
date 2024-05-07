#pragma once

#include <vector>
#include <memory>
#include "Utils.h"
#include "Expression.h"
#include "Statement.h"
#include "Typing.h"

class Variable : public Statement {
  public:
    std::string name;
    std::unique_ptr<Expression> value;
    Typing typing;
    bool is_constant;
    bool is_field = false;

    Variable();

    static PeekPtr<Variable> build(Stream &stream, const size_t &start_index);
    static PeekPtr<Variable> build_as_field(Stream &stream, const size_t &start_index);
    static PeekPtr<Variable> build_as_property(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};