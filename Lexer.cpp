#pragma once

#include <algorithm>
#include "Lexer.h"
#include "Utils.h"

std::map<Token::Kind, std::string> KIND = {
  {Token::Kind::IDENTIFIER, "Identifier"},
  {Token::Kind::LITERAL, "Literal"},
  {Token::Kind::MARKER, "Marker"},
  {Token::Kind::OPERATOR, "Operator"},
  {Token::Kind::KEYWORD, "Keyword"},
};

std::string get_kind_name(Token::Kind kind) {
  return KIND.at(kind);
}

std::map<std::string, Operator> OPERATOR = {
  {"=", Operator::ASSIGN},
  {"+", Operator::ADD},
  {"-", Operator::SUB},
  {"*", Operator::MUL},
  {"/", Operator::DIV},
  {"%", Operator::MOD},
  {"and", Operator::AND},
  {"or", Operator::OR},
  {"not", Operator::NOT},
};

std::map<std::string, Keyword> KEYWORD = {
  {"fn", Keyword::FN},
  {"var", Keyword::VAR},
  {"val", Keyword::VAL},
  {"struct", Keyword::STRUCT},
  {"fn", Keyword::FUNCTION},
  {"return", Keyword::RETURN},
  {"for", Keyword::FOR},
  {"if", Keyword::IF},
  {"else", Keyword::ELSE},
};

std::map<char, Marker> MARKER = {
  {'"', Marker::STR_QUOTE},
  {'#', Marker::STR_INJECTION},
  {'}', Marker::RIGHT_BRACE},
  {'{', Marker::LEFT_BRACE},
  {')', Marker::RIGHT_PARENTHESIS},
  {'(', Marker::LEFT_PARENTHESIS},
};

Token::Token(const char character) {
  if (not is_marker(character)) {
    throw std::runtime_error("DEV: Not a Marker");
  }

  kind = Kind::MARKER;
  data = character;
}

bool Token::is_given_kind(const Kind &kind) const {
  return this->kind == kind;
}

bool Token::is_given_keyword(const Keyword &keyword) const {
  if (kind != Kind::KEYWORD) {
    return false;
  }

  return KEYWORD.at(data) == keyword;
}

bool Token::is_given_marker(const Marker &marker) const {
  if (kind != Kind::MARKER) {
    return false;
  }

  return MARKER.at(data[0]) == marker;
}

Marker Token::get_marker(const char character) {
  return MARKER.at(character);
}

Operator Token::get_operator(const std::string &line) {
  return OPERATOR.at(line);
}

Keyword Token::get_keyword(const std::string &line) {
  return KEYWORD.at(line);
}

bool is_bool_literal(const std::string &line) {
  return line == "true" or line == "false";
}

bool is_float_literal(const std::string &line) {
  return std::count(line.begin(), line.end(), '.') == 1;
}

bool is_int_literal(const std::string &line) {
  return std::all_of(line.begin(), line.end(), [](char character) {
    return std::isdigit(character);
  });
}

bool Token::is_operator(const std::string &line) {
  return OPERATOR.find(line) != OPERATOR.end();
}

bool Token::is_keyword(const std::string &line) {
  return KEYWORD.find(line) != KEYWORD.end();
}

bool Token::is_marker(const char character) {
  return MARKER.find(character) != MARKER.end();
}

bool Token::is_valid_id_char(const char character) {
  return std::isalnum(character) or character == '_' or character == '$';
}

bool is_next(const std::string &line, const size_t start_index, std::function<bool(const char)> predicate) {
  if (start_index + 1 >= line.size()) {
    return false;
  }

  return predicate(line[start_index + 1]);
}

void Token::print() const {
  std::string kind = get_kind_name(this->kind);

  if (injections.empty()) {
    println(kind + " { data: " + data + " }");
  } else {
    println(kind + " {");
    println("  data: " + data);
    println("  injections: [" + Utils::join(injections, ", ") + "]");
    println("}");
  }
}

Peek<Token> Stream::peek(
  const size_t &start_index,
  const std::function<bool(const Token)> predicate
) const {
  Peek<Token> result;

  try {
    result.data = at(start_index + 1);
  } catch (const std::out_of_range &error) {
    throw std::runtime_error("DEV: Out of Range");
  }

  if (predicate(result.data)) {
    result.end_index = start_index + 1;
  } else {
    println("Unexpected Token: ");
    result.data.print();
    throw std::runtime_error("DEV: Unexepected Token");
  }

  return result;
}

void Stream::print() const {
  for (const Token &token : *this) token.print();
}

Peek<std::string> Lexer::handle_str_injection(const std::string &line, size_t start_index) {
  Peek<std::string> result;
  std::string injection;

  for (size_t i = start_index + 1; i < line.size(); i++) {
    const char character = line[i];

    if (not Token::is_valid_id_char(character)) {
      result.data = std::move(injection);
      result.end_index = i - 1;
      return result;
    }

    injection += character;
  }

  result.end_index = line.size() - 1;
  return result;
}

Result Lexer::handle_str_literal(const std::string &line, size_t start_index) {
  Result result;
  std::string literal;

  for (size_t i = start_index + 1; i < line.size(); i++) {
    const char character = line[i];

    if (character == '"') {
      result.data.kind = Token::Kind::LITERAL;
      result.data.data = std::move(literal);
      return result;
    }

    literal += character;

    if (character == '#') {
      bool is_next_alpha = is_next(line, i, [](char character) {
        return std::isalpha(character);
      });

      if (is_next_alpha) {
        Peek<std::string> injection = handle_str_injection(line, i);
        result.data.injections.push_back(injection.data);
        literal += injection.data;
        i = injection.end_index;
        continue;
      }
    }
  }

  throw std::runtime_error("DEV: Unterminated String Literal");
}

Token Lexer::handle_buffer(std::string &buffer) {
  Token token;

  if (Token::is_operator(buffer)) {
    token.kind = Token::Kind::OPERATOR;
  } else if (Token::is_keyword(buffer)) {
    token.kind = Token::Kind::KEYWORD;
  } else if (
    is_bool_literal(buffer) or
    is_float_literal(buffer) or
    is_int_literal(buffer)
  ) {
    token.kind = Token::Kind::LITERAL;  
  } else {
    token.kind = Token::Kind::IDENTIFIER;
  }

  token.data = std::move(buffer);
  buffer.clear();

  return token;
}

Stream Lexer::lex_ln(std::string line) {
  Stream stream;
  std::string buffer;

  line += ' ';
  
  for (size_t i = 0; i < line.size(); i++) {
    const char character = line[i];

    if (std::isspace(character)) {
      if (not buffer.empty()) {
        stream.push_back(handle_buffer(buffer));
      }

      continue;
    }

    if (Token::is_marker(character)) {
      if (not buffer.empty()) {
        stream.push_back(handle_buffer(buffer));
      }

      Marker marker = Token::get_marker(character);

      switch (marker) {
        case Marker::STR_QUOTE: {
          Result result = handle_str_literal(line, i);
          stream.push_back(result.data);
          i = result.end_index;
          break;
        }
        default: {
          Token token(character);
          stream.push_back(token);
          break;
        }
      }

      continue;
    }

    buffer += character;
  }

  return stream;
}

Stream Lexer::lex_file(const std::string &file_path) {
  Stream stream;
  
  Utils::each_line(file_path, [&](const std::string &line) {
    Stream line_stream = lex_ln(line);
    stream.insert(stream.end(), line_stream.begin(), line_stream.end());
  });

  return stream;
}