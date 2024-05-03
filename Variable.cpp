#pragma once

#include "Utils.h"
#include "Variable.h"

PeekPtr<Variable> Variable::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Variable> result;

  Token keyword = stream[start_index];
  Keyword kind = Token::get_keyword(keyword.data);
  
  if (kind != Keyword::VAR and kind != Keyword::VAL) {
    throw std::runtime_error("DEV: Expected 'var' or 'val' keyword");
  }

  result.data->is_constant = kind == Keyword::VAL;
  
  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.kind == Token::Kind::IDENTIFIER;
  });

  Peek<Token> assignment = stream.peek(name.end_index, [](const Token &token) {
    return token.kind == Token::Kind::OPERATOR and token.data == "=";
  });

  Peek<Token> value = stream.peek(assignment.end_index, [](const Token &token) {
    return token.kind == Token::Kind::LITERAL || token.kind == Token::Kind::IDENTIFIER;
  });

  result.data->name = name.data.data;
  result.data->value = value.data.data;
  result.end_index = value.end_index;
  return result;
}

PeekPtr<Variable> Variable::build_as_field(Stream &stream, const size_t &start_index) {
  PeekPtr<Variable> result;

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  Peek<Token> value = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  result.data->is_field = true;
  result.data->name = name.data.data;
  result.data->value = value.data.data;
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

  if (is_field) {
    println(indentation + "  type: " + value);
  } else {
    println(indentation + "  value: " + value);  
  }
  
  println(indentation + "}");
}