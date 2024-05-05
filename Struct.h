#pragma once

#include "Statement.h"
#include "Variable.h"

class Struct : public Statement {
  public:
    Type type = Type::STRUCT_DECLARATION;
    Kind kind = Kind::STATEMENT;
    std::string name;
    std::vector<std::unique_ptr<Variable>> fields;

    static bool is_struct_literal(const Stream &stream, const size_t &start_index);

    static PeekPtr<Struct> build(Stream &stream, const size_t &start_index);
    static PeekPtr<Object> build_as_struct_literal(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};