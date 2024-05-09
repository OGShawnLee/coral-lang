#pragma once

#include "Utils.h"
#include "Conditional.h"
#include "Parser.h"

PeekPtr<Else> Else::build(Stream &stream, const size_t &start_index) {
  if (not stream.at(start_index).is_given_keyword(Keyword::ELSE)) {
    throw std::runtime_error("DEV: Expected 'else' keyword");
  }

  PeekPtr<Else> result;

  if (stream.is_next(start_index, Keyword::IF)) {
    PeekPtr<If> if_statement = If::build(stream, start_index + 1);
    result.data->children.push_back(std::move(if_statement.data));
    result.end_index = if_statement.end_index;
  } else {
    PeekVectorPtr<Statement> body = Parser::build_block(stream, start_index + 1);
    result.data->children = std::move(body.data);
    result.end_index = body.end_index;
  }

  return result;
}

void Else::print(size_t indent) const {
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

PeekPtr<If> If::build(Stream &stream, const size_t &start_index) {
  if (not stream.at(start_index).is_given_keyword(Keyword::IF)) {
    throw std::runtime_error("DEV: Expected 'if' keyword");
  }

  PeekPtr<If> result;

  PeekPtr<Expression> condition = Expression::build(stream, start_index);
  PeekVectorPtr<Statement> body = Parser::build_block(stream, condition.end_index + 1);

  result.data->condition = std::move(condition.data);
  result.data->children = std::move(body.data);
  result.end_index = body.end_index;
 
  if (stream.is_next(body.end_index, Keyword::ELSE)) {
    PeekPtr<Else> else_block = Else::build(stream, result.end_index + 1);
    
    result.data->else_block = std::move(else_block.data);
    result.end_index = else_block.end_index;
  }

  return result;
}

void If::print(size_t indent) const {
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
  size_t index = left_brace.end_index;

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
      PeekPtr<Else> else_block = Else::build(stream, next.end_index);
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

  size_t index = start_index;

  while (index < stream.size()) {
    Token next = stream.get_next(index);

    if (next.is_given_marker(Marker::LEFT_BRACE)) {
      break;
    }

    if (next.is_given_marker(Marker::COMMA)) {
      index += 1;
      continue;
    }

    PeekPtr<Expression> condition = Expression::build(stream, index);
    result.data->conditions.push_back(std::move(condition.data));
    index = condition.end_index;
  }

  PeekVectorPtr<Statement> body = Parser::build_block(stream, index + 1);
  result.data->children = std::move(body.data);
  result.end_index = body.end_index;
  return result;
}

void When::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "When Statement {");

  if (not conditions.empty()) {
    println(indentation + "  conditions: [");
    for (const auto &condition : conditions) {
      println(indentation + "    " + condition->to_string(indent + 2));
    }
    println(indentation + "  ]");
  }

  if (not children.empty()) {
    println(indentation + "  children: [");
    for (const auto &child : children) {
      child->print(indent + 2);
    }
    println(indentation + "  ]");
  }

  println(indentation + "}");
}