#pragma once

#include <vector>
#include <memory>
#include "Utils.h"
#include "Statement.h"

class Variable : public Statement {
  public:
    Type type = Type::VARIABLE_DECLARATION;
    Kind kind = Kind::STATEMENT;
    std::string name;
    std::string value;
    bool is_constant;

    static PeekPtr<Variable> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};