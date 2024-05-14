#pragma once

#include "Utils.h"
#include "Expression.cpp"
#include "Variable.h"
#include "Typing.cpp"

Variable::Variable() {
  type = Type::VARIABLE_DECLARATION;
  kind = Kind::STATEMENT;
}

PeekPtr<Variable> Variable::build(Stream &stream, const size_t &start_index) {
  Keyword keyword = Token::get_keyword(
    stream.at(start_index).data
  );
  
  if (keyword != Keyword::VAL and keyword != Keyword::VAR) {
    throw std::runtime_error("DEV: Expected 'var' or 'val' keyword");
  }

  PeekPtr<Variable> result;

  result.data->is_constant = keyword == Keyword::VAL;
  
  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.kind == Token::Kind::IDENTIFIER;
  });

  Peek<Token> assignment = stream.peek(name.end_index, [](const Token &token) {
    return token.kind == Token::Kind::OPERATOR and token.data == "=";
  });

  PeekPtr<Expression> value = Expression::build(stream, assignment.end_index);
  
  result.data->typing.from_expression(value.data);
  result.data->name = name.data.data;
  result.data->value = std::move(value.data);
  result.end_index = value.end_index;
  return result;
}

PeekPtr<Variable> Variable::build_as_field(Stream &stream, const size_t &start_index) {
  PeekPtr<Variable> result;

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  // field <type> | field = <value>
  Peek<Token> next = stream.peek(name.end_index, [](const Token &token) {
    return 
      token.is_given_kind(Token::Kind::IDENTIFIER) ||
      token.is_given_literal(Token::Literal::ARRAY) || 
      token.is_given_operator(Operator::ASSIGN) ||
      token.is_given_keyword(Keyword::FUNCTION);
  });

  if (next.data.is_given_operator(Operator::ASSIGN)) {
    // field = <value>
    PeekPtr<Expression> value = Expression::build(stream, next.end_index);

    result.data->typing.from_expression(value.data);
    result.data->value = std::move(value.data);
    result.end_index = value.end_index;
  } else {
    // field <type>
    Peek<Typing> typing = Typing::build(stream, next.end_index - 1);
    result.data->typing = std::move(typing.data);
    result.end_index = typing.end_index;
  }

  result.data->name = name.data.data;
  result.data->is_field = true;
  return result;
}

PeekPtr<Variable> Variable::build_as_property(Stream &stream, const size_t &start_index) {
  PeekPtr<Variable> result;
  
  Peek<Token> name = stream.peek(start_index, Token::Kind::IDENTIFIER);
  Peek<Token> colon = stream.peek(name.end_index, Marker::COLON);

  PeekPtr<Expression> value = Expression::build(stream, colon.end_index);

  result.data->typing.from_expression(value.data);
  result.data->name = name.data.data;
  result.data->value = std::move(value.data);
  result.end_index = value.end_index;
  return result;
}

void Variable::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);

  if (is_field) {
    println(indentation + "Field {");
  } else if (is_constant) {
    println(indentation + "Constant {");
  } else {
    println(indentation + "Variable {");
  }

  println(indentation + "  name: " + name);

  if (value != nullptr) {
    println(
      indentation + (is_field ? "  default: " : "  value: ") + value->to_string(indent + 1)
    );
  }
  
  println(indentation + "  type: " + typing.to_string(indent + 1));  

  println(indentation + "}");
}