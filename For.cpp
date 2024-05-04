#pragma once

#include "For.h"

PeekPtr<For> For::build(Stream &stream, const size_t &start_index) {
  PeekPtr<For> result;

  Token keyword = stream[start_index];
  if (not keyword.is_given_keyword(Keyword::FOR)) {
    throw std::runtime_error("DEV: Expected 'for' keyword");
  }

  Peek<Token> next = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER, Token::Kind::LITERAL, Token::Kind::MARKER);
  });
  
  // for {}
  if (next.data.is_given_marker(Marker::LEFT_BRACE)) {
    result.data->variant = For::Variant::INFINITE;
    result.end_index = next.end_index;
    return result;
  }

  result.data->index = next.data.data;
  result.end_index = next.end_index;

  if (stream.is_next(next.end_index, Keyword::IN)) {
    Peek<Token> limit = stream.peek(next.end_index + 1, [](const Token &token) {
      return token.is_given_kind(Token::Kind::IDENTIFIER, Token::Kind::LITERAL);
    });

    // for <index> in <limit> {}
    result.data->limit = limit.data.data;
    result.data->variant = For::Variant::FOR_IN;
    result.end_index = limit.end_index;
  } else {
    // for <index> {}
    result.data->variant = For::Variant::TIMES;
  }

  return result;
}

void For::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "For {");
  if (variant == Variant::INFINITE) {
    println(indentation + "  variant: Infinite");
  } else if (variant == Variant::TIMES) {
    println(indentation + "  index: " + index);
    println(indentation + "  variant: Times");
  } else {
    println(indentation + "  index: " + index);
    println(indentation + "  limit: " + limit);
    println(indentation + "  variant: For");
  }
  println(indentation + "}");
}