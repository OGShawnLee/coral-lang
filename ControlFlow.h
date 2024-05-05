#pragma once

#include "Utils.h"
#include "Statement.h"
#include "Expression.h"

class ELSE : public Statement {
  public:
    static PeekPtr<ELSE> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};

class IF : public Statement {
  public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<ELSE> else_block;

    static PeekPtr<IF> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};

