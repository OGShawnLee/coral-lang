#pragma once

#include "Utils.h"
#include "Struct.h"

PeekPtr<Struct> Struct::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Struct> result;

  Token keyword = stream[start_index];
  if (not keyword.is_given_keyword(Keyword::STRUCT)) {
    throw std::runtime_error("DEV: Expected 'struct' keyword");
  }

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  Peek<Token> open_brace = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_BRACE);
  });

  size_t index = open_brace.end_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return token.is_given_marker(Marker::RIGHT_BRACE) || token.is_given_kind(Token::Kind::IDENTIFIER);
    });

    if (next.data.is_given_marker(Marker::RIGHT_BRACE)) {
      result.data->name = name.data.data;
      result.end_index = next.end_index;
      
      if (result.data->fields.empty()) {
        throw std::runtime_error("USER: Struct " + name.data.data + " has no fields");      
      }
      
      return result;
    }
    
    PeekPtr<Variable> field = Variable::build_as_field(stream, index);
    result.data->fields.push_back(std::move(field.data));
    index = field.end_index;
  }

  throw std::runtime_error("USER: Unterminated Struct " + name.data.data + " Declaration");
}

void Struct::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Struct {");
  println(indentation + "  name: " + name);
  println(indentation + "  fields: [");

  for (const std::unique_ptr<Variable> &field : fields) {
    field->print(indent + 2);
  }

  println(indentation + "  ]");
  println(indentation + "}");
}