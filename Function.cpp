#pragma once

#include "Utils.h"
#include "Function.h"

PeekPtr<Function> Function::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Function> result;

  Token keyword = stream[start_index];
  if (not keyword.is_given_keyword(Keyword::FUNCTION)) {
    throw std::runtime_error("DEV: Expected 'fn' keyword");
  }

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  Peek<Token> open_parenthesis = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_PARENTHESIS);
  });
  
  size_t index = open_parenthesis.end_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_marker(Marker::RIGHT_PARENTHESIS, Marker::COMMA) || 
        token.is_given_kind(Token::Kind::IDENTIFIER);
    });

    if (next.data.is_given_marker(Marker::COMMA)) {
      index = next.end_index;
      continue;
    }

    if (next.data.is_given_marker(Marker::RIGHT_PARENTHESIS)) {
      result.data->name = name.data.data;
      result.end_index = next.end_index;
      return result;
    }
    
    PeekPtr<Variable> parameter = Variable::build_as_field(stream, index);
    result.data->parameters.push_back(std::move(parameter.data));
    index = parameter.end_index;
  }

  throw std::runtime_error("USER: Unterminated Function " + name.data.data + " Declaration");
}


void Function::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Function {");
  println(indentation + "  name: " + name);
  println(indentation + "  parameters: [");

  for (const std::unique_ptr<Variable> &parameter : parameters) {
    parameter->print(indent + 2);
  }

  println(indentation + "  ]");
  println(indentation + "}");
}