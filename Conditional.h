#pragma once

#include "Utils.h"
#include "Statement.h"
#include "Expression.h"

class Else : public Statement {
  public:
    bool is_match_else; 

    static PeekPtr<Else> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};

class If : public Statement {
  public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Else> else_block;

    static PeekPtr<If> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};

class When : public Statement {
  public:
    std::vector<std::unique_ptr<Expression>> conditions;

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