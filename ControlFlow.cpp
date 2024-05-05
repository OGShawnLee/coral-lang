#pragma once

#include "Utils.h"
#include "ControlFlow.h"
#include "Parser.h"

PeekPtr<ELSE> ELSE::build(Stream &stream, const size_t &start_index) {
  if (not stream.at(start_index).is_given_keyword(Keyword::ELSE)) {
    throw std::runtime_error("DEV: Expected 'else' keyword");
  }

  PeekPtr<ELSE> result;
  PeekVectorPtr<Statement> body = Parser::build_block(stream, start_index + 1);

  result.data->children = std::move(body.data);
  result.end_index = body.end_index;

  return result;
}

void ELSE::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Else Statement {");

  if (not children.empty()) {
    println(indentation + "  children: [");
    for (const auto &child : children) {
      child->print(indent + 2);
    }
    println(indentation + "  ]");
  }

  println(indentation + "}");
}

PeekPtr<IF> IF::build(Stream &stream, const size_t &start_index) {
  if (not stream.at(start_index).is_given_keyword(Keyword::IF)) {
    throw std::runtime_error("DEV: Expected 'if' keyword");
  }

  PeekPtr<IF> result;

  PeekPtr<Expression> condition = Expression::build(stream, start_index);
  PeekVectorPtr<Statement> body = Parser::build_block(stream, condition.end_index + 1);

  if (stream.is_next(body.end_index, Keyword::ELSE)) {
    PeekPtr<ELSE> else_block = ELSE::build(stream, body.end_index + 1);
    
    result.data->else_block = std::move(else_block.data);
    result.end_index = else_block.end_index;
  }

  result.data->condition = std::move(condition.data);
  result.data->children = std::move(body.data);

  return result;
}

void IF::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "If Statement {");
  println(indentation + "  condition: " + condition->to_string(indent + 1));

  if (not children.empty()) {
    println(indentation + "  children: [");
    for (const auto &child : children) {
      child->print(indent + 2);
    }
    println(indentation + "  ]");

  }
  println(indentation + "}");
  
  if (else_block) {
    else_block->print(indent);
  }
}