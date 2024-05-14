#pragma once

#include "Expression.h"
#include "Typing.h"
#include "Parser.h"

Block::Block() {
  variant = Variant::BLOCK;
}

bool Block::is_block(Stream &stream, const size_t &start_index) {
  // block <struct> <fn> <arr_literal> {} 
  if (stream.is_next(start_index, Keyword::BLOCK)) return true;

  // <type> {}
  return 
    not stream.at(start_index).is_given_kind(Token::Kind::KEYWORD) &&
    stream.is_next(start_index, [](const Token &token) {
      return token.is_given_kind(Token::Kind::IDENTIFIER);
    }) &&
    stream.is_next(start_index + 1, [](const Token &token) {
      return token.is_given_marker(Marker::LEFT_BRACE);
    });
}

PeekPtr<Block> Block::build(Stream &stream, const size_t &start_index) {
  if (not is_block(stream, start_index)) {
    throw std::runtime_error("DEV: Not a Block");
  }

  PeekPtr<Block> result;

  Token next = stream.get_next(start_index);
  if (next.kind == Token::Kind::KEYWORD) {
    result.end_index = start_index + 1;
  } else result.end_index = start_index;    

  Peek<Typing> typing = Typing::build(stream, result.end_index);
  typing.data.print();
  PeekVectorPtr<Statement> body = Parser::build_block(stream, typing.end_index + 1);

  result.end_index = body.end_index;
  result.data->children = std::move(body.data);
  result.data->typing = std::move(typing.data);

  return result;
}

std::string Block::to_string(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  std::string result = "Block {\n";
  result += indentation + "  typing: " + typing.to_string(indent + 1) + "\n";
  result += indentation + "}";
  return result;
}

void Block::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Block {");
  println(indentation + "  typing: " + typing.to_string(indent + 1));

  for (const auto &child : children) {
    child->print(indent + 1);
  }

  println(indentation + "}");
}