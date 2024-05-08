#pragma once

#include "Utils.h"
#include "Expression.h"
#include "Statement.h"

class Expression;

class Typing {
  public:  
    Token::Literal data;
    std::string value;
    std::vector<Typing> children;

    void from_expression(const std::unique_ptr<Expression> &expression);

    static Peek<Typing> build(Stream &stream, const size_t &start_index);

    static Token::Literal infer_built_in_type_from_string(std::string data);
    static std::string infer_built_in_type(Token::Literal literal);
    
    void print(size_t indent = 0) const;

    std::string to_string(size_t indent = 0, const bool &is_child = false) const;
};