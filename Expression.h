#pragma once

#include "Utils.h"
#include "Statement.h"

class Expression : public Statement {
  public:
    enum class Type {
      LITERAL,
      IDENTIFIER,
    };

    Type type;
    std::string value;

    static bool is_expression(Stream &stream, const size_t &start_index);

    static PeekPtr<Expression> build(Stream &stream, const size_t &start_index);

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};

class BinaryExpression : public Expression {
  public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string operation;

    static bool is_binary_expression(Stream &stream, const size_t &start_index);

    static PeekPtr<BinaryExpression> build(Stream &stream, const size_t &start_index);

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};