#pragma once

#include "Utils.h"
#include "Function.cpp"
#include "Struct.h"

bool Struct::is_struct_literal(const Stream &stream, const size_t &start_index) {
  return 
    // prevent matching <keyword> <identifier< {}  
    not stream.at(start_index).is_given_kind(Token::Kind::KEYWORD) &&
    stream.is_next(start_index, [](const Token &token) {
      return token.is_given_kind(Token::Kind::IDENTIFIER) && isupper(token.data[0]);
    }) &&
    stream.is_next(start_index + 1, [](const Token &token) {
      return token.is_given_marker(Marker::LEFT_BRACE);
    });
}

PeekPtr<Struct> Struct::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Struct> result;

  Token keyword = stream[start_index];
  if (not keyword.is_given_keyword(Keyword::STRUCT)) {
    throw std::runtime_error("DEV: Expected 'struct' keyword");
  }

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  if (not isupper(name.data.data[0])) {
    throw std::runtime_error(
      "USER: Struct name must start with an uppercase letter (" + name.data.data + ")"
    );
  }

  Peek<Token> open_brace = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_BRACE);
  });

  size_t index = open_brace.end_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_marker(Marker::RIGHT_BRACE) || 
        token.is_given_kind(Token::Kind::IDENTIFIER) ||
        token.is_given_keyword(Keyword::FUNCTION);
    });

    if (next.data.is_given_keyword(Keyword::FUNCTION)) {
      PeekPtr<Function> method = Function::build(stream, next.end_index);
      result.data->methods.push_back(std::move(method.data));
      index = method.end_index;
      continue;
    }

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

PeekPtr<Object> Struct::build_as_struct_literal(Stream &stream, const size_t &start_index) {
  PeekPtr<Object> result;

  Peek<Token> name = stream.peek(start_index, Token::Kind::IDENTIFIER);
  Peek<Token> brace = stream.peek(name.end_index, Marker::LEFT_BRACE);

  size_t index = brace.end_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_marker(Marker::RIGHT_BRACE, Marker::COMMA) || 
        token.is_given_kind(Token::Kind::IDENTIFIER);
    });

    if (next.data.is_given_marker(Marker::COMMA)) {
      index = next.end_index;
      continue;
    }

    if (next.data.is_given_marker(Marker::RIGHT_BRACE)) {
      result.data->name = name.data.data;
      result.end_index = next.end_index;
      return result;
    }

    PeekPtr<Variable> property = Variable::build_as_property(stream, index);
    result.data->properties.push_back(std::move(property.data));
    index = property.end_index;
  }

  return result;
}

void Struct::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Struct {");
  println(indentation + "  name: " + name);
  println(indentation + "  fields: [");

  for (const std::unique_ptr<Variable> &field : fields) {
    field->print(indent + 2);
  }

  if (not methods.empty()) {
    println(indentation + "  ]");
    println(indentation + "  methods: [");

    for (const std::unique_ptr<Function> &method : methods) {
      method->print(indent + 2);
    }
  }

  println(indentation + "  ]");
  println(indentation + "}");
}

void Object::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Struct Literal {");
  println(indentation + "  name: " + name);
  println(indentation + "  properties: [");

  for (const std::unique_ptr<Variable> &field : properties) {
    field->print(indent + 2);
  }

  println(indentation + "  ]");
  println(indentation + "}");
}

std::string Object::to_string(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  std::string result = "Struct Literal {\n";
  result += indentation + "  name: " + name + "\n";
  result += indentation + "  properties: [\n";

  for (const std::unique_ptr<Variable> &field : properties) {
    result += "    " + indentation + field->name + ": " + field->value->to_string(indent + 2) + "\n";
  }

  result += indentation + "  ]\n";
  result += indentation + "}\n";
  return result;
}