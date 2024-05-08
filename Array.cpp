#pragma once

#include "Utils.h"
#include "Expression.h"
#include "Struct.cpp"
#include "Typing.cpp"

bool Array::is_arr_literal(Stream &stream, const size_t &start_index) {
  return stream.is_next(start_index, [](const Token token) {
    return 
      token.is_given_kind(Token::Kind::LITERAL) &&
      token.data == "[]";
  });
}

PeekPtr<Array> Array::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Array> result;

  if (not is_arr_literal(stream, start_index)) {
    throw std::runtime_error("DEV: Not an Array");
  }

  Peek<Typing> typing = Typing::build(stream, start_index);

  bool has_init = stream.is_next(typing.end_index, [](const Token token) {
    return token.is_given_marker(Marker::LEFT_BRACE);
  });

  result.data->typing = std::move(typing.data);
  result.data->value = "[]";
  result.end_index = typing.end_index;
  
  if (has_init) {
    Peek<MapPtr<Expression>> properties = Object::get_given_properties(
      stream, 
      result.end_index, 
      { "len", "init" }
    );

    if (Utils::has_key(properties.data, "len")) {
      result.data->len = std::move(properties.data.at("len"));
    }

    if (Utils::has_key(properties.data, "init")) {
      if (not result.data->len) {
        throw std::runtime_error("USER: Array Literal has init but is missing len");
      } 

      result.data->init = std::move(properties.data.at("init"));
    }

    result.end_index = properties.end_index;
  }

  result.data->variant = Expression::Variant::LITERAL;
  return result;
}

void Array::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Array Literal {");
  println(indentation + "  type: " + typing.to_string(indent + 1));
  println(indentation + "  value: " + value);

  if (len) {
    println(indentation + "  len: " + len->to_string(indent + 1));
  }

  if (init) {
    println(indentation + "  init: " + init->to_string(indent + 1));
  }

  println(indentation + "}");
}

std::string Array::to_string(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  std::string result = "Array Literal {\n";
  result += indentation + "  type: " + typing.to_string(indent + 1);
  result += indentation + "  value: " + value + "\n";

  if (len) {
    result += indentation + "  len: " + len->to_string(indent + 2) + "\n";
  }

  if (init) {
    result += indentation + "  init: " + init->to_string(indent + 1) + "\n";
  }

  result += indentation + "}";

  return result;
}