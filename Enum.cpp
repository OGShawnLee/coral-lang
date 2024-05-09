#pragma once

#include "Utils.h"
#include "Enum.h"
#include "Function.cpp"

PeekPtr<Enum> Enum::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Enum> result;

  if (not stream.at(start_index).is_given_keyword(Keyword::ENUM)) {
    throw std::runtime_error("DEV: Expected 'enum' keyword");
  }

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });
   
  if (not isupper(name.data.data[0])) {
    throw std::runtime_error(
      "USER: Enum name must start with an uppercase letter (" + name.data.data + ")"
    );
  }

  Peek<Token> opening = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_BRACE);
  });

  size_t index = opening.end_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_marker(Marker::RIGHT_BRACE, Marker::COMMA) || 
        token.is_given_kind(Token::Kind::IDENTIFIER) || 
        token.is_given_keyword(Keyword::FUNCTION);
    });

    if (next.data.kind == Token::Kind::KEYWORD) {
      PeekPtr<Function> method = Function::build(stream, next.end_index);

      result.data->methods.push_back(std::move(method.data));
      index = method.end_index;
      continue;
    }

    if (next.data.is_given_marker(Marker::COMMA)) {
      index = next.end_index;
      continue;
    }

    if (next.data.is_given_marker(Marker::RIGHT_BRACE)) {
      result.data->name = name.data.data;
      result.end_index = next.end_index;
      
      if (result.data->values.empty()) {
        throw std::runtime_error("USER: Enum " + name.data.data + " has no values");
      }

      return result;
    }

    if (not Utils::is_all_upper(next.data.data)) {
      throw std::runtime_error(
        "USER: Enum value must be all uppercase (" + next.data.data + ")"
      );
    }

    result.data->values.push_back(next.data.data);
    index = next.end_index;
  }

  throw std::runtime_error("USER: Unterminated Enum " + name.data.data + " Declaration");
}

void Enum::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);

  println(indentation + "Enum: " + name + " {");
  println(indentation+ "  values: {");
  for (const std::string &value : values) {
    println(indentation + "    " + value);
  }
  println(indentation + "  }");
  if (not methods.empty()) {
    println(indentation + "  methods: {");
    for (const auto &fun : methods) {
      fun->print(indent + 2);
    }
    println(indentation + "  }");
  }
  println(indentation + "}");
}

