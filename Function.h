#pragma once

#include "Utils.h"
#include "Expression.h"
#include "Statement.h"
#include "Variable.h"
#include "Typing.h"

class Function : public Statement {
  public:
    std::string name;
    std::vector<std::unique_ptr<Variable>> parameters;
    Typing typing;

    Function();

    static bool is_fn_call(Stream &stream, const size_t &start_index);

    static bool is_lambda(Stream &stream, const size_t &start_index);

    static PeekPtr<Function> build(Stream &stream, const size_t &start_index);

    static PeekPtr<Expression> build_as_fn_call(Stream &stream, const size_t &start_index);

    static Peek<Typing> build_as_fn_type(Stream &stream, const size_t &start_index);
    
    static PeekPtr<Lambda> build_as_lambda(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};