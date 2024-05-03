#pragma once

#include "Parser.h"
#include "Lexer.cpp"
#include "Variable.cpp"

Statement Parser::parse(const std::string &file_path) {
  Statement program;
  Stream stream = Lexer::lex_file(file_path);
  program.kind = Statement::Kind::PROGRAM;

  for (size_t i = 0; i < stream.size(); i++) {
    Token token = stream[i];

    if (token.kind == Token::Kind::KEYWORD) {
      Keyword keyword = Token::get_keyword(token.data);

      if (keyword == Keyword::VAR || keyword == Keyword::VAL) {
        PeekPtr<Variable> variable = Variable::build(stream, i);
        program.children.push_back(std::move(variable.data));
        i = variable.end_index;
      }
    }
  }

  return program;
}