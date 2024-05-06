#pragma once

#include "Utils.h"
#include "Statement.h"
#include "Expression.h"

class ELSE : public Statement {
  public:
    Type type = Type::ELSE_STATEMENT;

    static PeekPtr<ELSE> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};

class IF : public Statement {
  public:
    Type type = Type::IF_STATEMENT;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<ELSE> else_block;

    static PeekPtr<IF> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};

class When : public Statement {
  public:
    std::unique_ptr<Expression> condition;

    When();

    static PeekPtr<When> build(Stream &stream, const size_t &start_index);

    void print(size_t indent = 0) const;
};

class Match : public Statement {
  public:
    std::unique_ptr<Expression> condition;

    Match();

    static PeekPtr<Match> build(Stream &stream, const size_t &start_index);

    void print(size_t indent = 0) const;
};