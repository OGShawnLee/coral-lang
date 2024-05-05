#pragma once

#include "Utils.h"
#include "Variable.h"
#include "Statement.h"

class Variable;

class Expression : public Statement {
  public:
    enum class Type {
      BINARY,
        ASSIGNMENT,
        PROPERTY_ACCESS,
      FUNCTION_CALL,
      LITERAL,
      IDENTIFIER,
    };

    Type type;
    std::string value;
    std::vector<std::unique_ptr<Expression>> arguments;

    static bool is_expression(Stream &stream, const size_t &start_index);

    static PeekPtr<Expression> build(Stream &stream, const size_t &start_index, const bool &with_binary = true);

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};

class BinaryExpression : public Expression {
  public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string operation;

    static bool is_binary_expression(Stream &stream, const size_t &start_index);

    static PeekPtr<BinaryExpression> build(
      Stream &stream, 
      const size_t &start_index,
      const bool &with_left = true 
    );

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};

// Struct Literal
class Object : public Expression {
  public:
    std::string name;
    std::vector<std::unique_ptr<Variable>> properties;

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};

class Array : public Expression {
  public:
    std::string typing;

    static bool is_arr_literal(Stream &stream, const size_t &start_index);

    static PeekPtr<Array> build(Stream &stream, const size_t &start_index);

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};