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

Match::Match() {
  type = Type::MATCH_STATEMENT;
}

PeekPtr<Match> Match::build(Stream &stream, const size_t &start_index) {
  if (not stream.at(start_index).is_given_keyword(Keyword::MATCH)) {
    throw std::runtime_error("DEV: Expected 'match' keyword");
  }

  PeekPtr<Match> result;
  PeekPtr<Expression> condition = Expression::build(stream, start_index);
  Peek<Token> left_brace = stream.peek(condition.end_index, Marker::LEFT_BRACE);
  size_t index = condition.end_index + 1;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_keyword(Keyword::WHEN, Keyword::ELSE) || 
        token.is_given_marker(Marker::RIGHT_BRACE);
    });

    if (next.data.is_given_marker(Marker::RIGHT_BRACE)) {
      if (result.data->children.empty()) {
        throw std::runtime_error("DEV: Empty 'match' statement");
      }

      result.data->condition = std::move(condition.data);
      result.end_index = next.end_index;
      return result;
    }

    if (next.data.is_given_keyword(Keyword::WHEN)) {
      PeekPtr<When> when = When::build(stream, next.end_index);
      result.data->children.push_back(std::move(when.data));
      index = when.end_index;
    } else {
      PeekPtr<ELSE> else_block = ELSE::build(stream, next.end_index);
      result.data->children.push_back(std::move(else_block.data));
      index = else_block.end_index;
    }
  }

  throw std::runtime_error("DEV: Unterminated 'match' statement");
}

void Match::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Match Statement {");
  println(indentation + "  condition: " + condition->to_string(indent + 1));

  if (not children.empty()) {
    println(indentation + "  children: [");
    for (const auto &child : children) {
      child->print(indent + 2);
    }
    println(indentation + "  ]");
  }

  println(indentation + "}");
}

When::When() {
  type = Type::WHEN_STATEMENT;
}

PeekPtr<When> When::build(Stream &stream, const size_t &start_index) {
  if (not stream.at(start_index).is_given_keyword(Keyword::WHEN)) {
    throw std::runtime_error("DEV: Expected 'when' keyword");
  }

  PeekPtr<When> result;
  PeekPtr<Expression> condition = Expression::build(stream, start_index);
  PeekVectorPtr<Statement> body = Parser::build_block(stream, condition.end_index + 1);

  result.data->children = std::move(body.data);
  result.data->condition = std::move(condition.data);
  result.end_index = body.end_index;
  return result;
}

void When::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "When Statement {");
  println(indentation + "  condition: " + condition->to_string(indent + 1));

  if (not children.empty()) {
    println(indentation + "  children: [");
    for (const auto &child : children) {
      child->print(indent + 2);
    }
    println(indentation + "  ]");
  }

  println(indentation + "}");
}