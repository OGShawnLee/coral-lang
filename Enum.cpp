#pragma once

#include "Utils.h"
#include "Enum.h"

PeekPtr<Enum> Enum::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Enum> result;

  Token keyword = stream[start_index];
  if (not keyword.is_given_keyword(Keyword::ENUM)) {
    throw std::runtime_error("DEV: Expected 'enum' keyword");
  }

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  Peek<Token> opening = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_BRACE);
  });

  size_t index = opening.end_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return token.is_given_marker(Marker::RIGHT_BRACE, Marker::COMMA) || token.is_given_kind(Token::Kind::IDENTIFIER);
    });

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

    result.data->values.push_back(next.data.data);
    index = next.end_index;
  }

  throw std::runtime_error("USER: Unterminated Enum " + name.data.data + " Declaration");
}

void Enum::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);

  std::cout << indentation << "Enum: " << name << " {\n";
  for (const std::string &value : values) {
    std::cout << indentation << "  " << value << '\n';
  }
  std::cout << indentation << "}\n";
}

