#pragma once

#include "Parser.h"
#include "Lexer.cpp"
#include "Enum.cpp"
#include "For.cpp"
#include "Expression.cpp"
#include "Function.cpp"
#include "Struct.cpp"
#include "Variable.cpp"

PeekVectorPtr<Statement> Parser::build_block(
  Stream &stream, 
  const size_t &start_index,
  bool is_main_program
) {
  if (not is_main_program) {
    if (not stream.at(start_index).is_given_marker(Marker::LEFT_BRACE)) { 
      throw std::runtime_error("DEV: Block not opened");
    }
  }

  PeekVectorPtr<Statement> block;

  for (size_t i = start_index + not is_main_program; i < stream.size(); i++) {
    Token token = stream[i];

    if (token.kind == Token::Kind::KEYWORD) {
      Keyword keyword = Token::get_keyword(token.data);

      if (keyword == Keyword::ENUM) {
        PeekPtr<Enum> enumeration = Enum::build(stream, i);
        block.data.push_back(std::move(enumeration.data));
        i = enumeration.end_index;
      }

      if (keyword == Keyword::FOR) {
        PeekPtr<For> loop = For::build(stream, i);
        block.data.push_back(std::move(loop.data));
        i = loop.end_index;
      }

      if (keyword == Keyword::FUNCTION) {
        PeekPtr<Function> function = Function::build(stream, i);
        block.data.push_back(std::move(function.data));
        i = function.end_index;
      }

      if (keyword == Keyword::STRUCT) {
        PeekPtr<Struct> structure = Struct::build(stream, i);
        block.data.push_back(std::move(structure.data));
        i = structure.end_index;
      }

      if (keyword == Keyword::VAR || keyword == Keyword::VAL) {
        PeekPtr<Variable> variable = Variable::build(stream, i);
        block.data.push_back(std::move(variable.data));
        i = variable.end_index;
      }
    }

    if (token.is_given_kind(Token::Kind::IDENTIFIER, Token::Kind::LITERAL)) {
      if (Expression::is_expression(stream, i - 1)) {
        PeekPtr<Expression> expression = Expression::build(stream, i - 1);
        block.data.push_back(std::move(expression.data));
        i = expression.end_index;
      }
    }

    if (token.is_given_marker(Marker::RIGHT_BRACE)) {
      block.end_index = i;
      return block;
    }
  }

  if (not is_main_program) {
    throw std::runtime_error("USER: Block not closed");
  }

  block.end_index = stream.size();
  return block;
}

Statement Parser::parse(const std::string &file_path) {
  Statement program;
  Stream stream = Lexer::lex_file(file_path);
  PeekVectorPtr<Statement> block = build_block(stream, 0, true);
  
  program.children = std::move(block.data);
  program.kind = Statement::Kind::PROGRAM;

  return program;
}