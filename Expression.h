#pragma once

#include "Utils.h"
#include "Variable.h"
#include "Statement.h"

template <typename T>
using MapPtr = std::map<std::string, std::unique_ptr<T>>;

class Variable;

class Expression : public Statement {
  public:
    enum class Variant {
      BINARY,
        ASSIGNMENT,
        PROPERTY_ACCESS,
      FUNCTION_CALL,
      LITERAL,
      IDENTIFIER,
    };

    Variant variant;
    Token::Literal literal;
    std::string value;
    std::vector<std::unique_ptr<Expression>> arguments;

    Expression();

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

    static Peek<MapPtr<Expression>> get_given_properties(
      Stream &stream,
      const size_t &start_index,
      const std::vector<std::string> &properties
    );

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};

class Array : public Expression {
  public:
    std::string typing;
    std::unique_ptr<Expression> len;
    std::unique_ptr<Expression> init;

    static bool is_arr_literal(Stream &stream, const size_t &start_index);

    static PeekPtr<Array> build(Stream &stream, const size_t &start_index);

    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};

class Lambda : public Expression {
  public:
    std::vector<std::unique_ptr<Variable>> parameters;
    
    void print(size_t indent = 0) const override;

    virtual std::string to_string(size_t indent = 0) const;
};