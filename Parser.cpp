#pragma once

#include "Parser.h"
#include "Lexer.cpp"
#include "Enum.cpp"
#include "For.cpp"
#include "Function.cpp"
#include "Struct.cpp"
#include "Variable.cpp"

Statement Parser::parse(const std::string &file_path) {
  Statement program;
  Stream stream = Lexer::lex_file(file_path);
  program.kind = Statement::Kind::PROGRAM;

  for (size_t i = 0; i < stream.size(); i++) {
    Token token = stream[i];

    if (token.kind == Token::Kind::KEYWORD) {
      Keyword keyword = Token::get_keyword(token.data);

      if (keyword == Keyword::ENUM) {
        PeekPtr<Enum> enumeration = Enum::build(stream, i);
        program.children.push_back(std::move(enumeration.data));
        i = enumeration.end_index;
      }

      if (keyword == Keyword::FOR) {
        PeekPtr<For> loop = For::build(stream, i);
        program.children.push_back(std::move(loop.data));
        i = loop.end_index;
      }

      if (keyword == Keyword::FUNCTION) {
        PeekPtr<Function> function = Function::build(stream, i);
        program.children.push_back(std::move(function.data));
        i = function.end_index;
      }

      if (keyword == Keyword::STRUCT) {
        PeekPtr<Struct> structure = Struct::build(stream, i);
        program.children.push_back(std::move(structure.data));
        i = structure.end_index;
      }

      if (keyword == Keyword::VAR || keyword == Keyword::VAL) {
        PeekPtr<Variable> variable = Variable::build(stream, i);
        program.children.push_back(std::move(variable.data));
        i = variable.end_index;
      }
    }
  }

  return program;
}