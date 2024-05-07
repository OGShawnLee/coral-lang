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

std::map<std::string, BinaryOperator> BINARY_OPERATOR = {
  {"=", BinaryOperator::ASSIGN},
  {"+", BinaryOperator::ADD},
  {"-", BinaryOperator::SUB},
  {"*", BinaryOperator::MUL},
  {"/", BinaryOperator::DIV},
  {"%", BinaryOperator::MOD},
  {"and", BinaryOperator::AND},
  {"or", BinaryOperator::OR},
  {":", BinaryOperator::COLON},
};

std::map<std::string, Keyword> KEYWORD = {
  {"var", Keyword::VAR},
  {"val", Keyword::VAL},
  {"enum", Keyword::ENUM},
  {"struct", Keyword::STRUCT},
  {"fn", Keyword::FUNCTION},
  {"return", Keyword::RETURN},
  {"for", Keyword::FOR},
  {"in", Keyword::IN},
  {"if", Keyword::IF},
  {"else", Keyword::ELSE},
  {"continue", Keyword::CONTINUE},
  {"break", Keyword::BREAK},
  {"match", Keyword::MATCH},
  {"when", Keyword::WHEN},
};

std::map<char, Marker> MARKER = {
  {'"', Marker::STR_QUOTE},
  {'#', Marker::STR_INJECTION},
  {'}', Marker::RIGHT_BRACE},
  {'{', Marker::LEFT_BRACE},
  {')', Marker::RIGHT_PARENTHESIS},
  {'(', Marker::LEFT_PARENTHESIS},
  {',', Marker::COMMA},
  {':', Marker::COLON},
  {']', Marker::RIGHT_BRACKET},
  {'[', Marker::LEFT_BRACKET},
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

bool Token::is_given_kind(const Kind &kind_a, const Kind &kind_b) const {
  return kind == kind_a or kind == kind_b;
}

bool Token::is_given_kind(const Kind &kind_a, const Kind &kind_b, const Kind &kind_c) const {
  return kind == kind_a or kind == kind_b or kind == kind_c;
}

bool Token::is_given_keyword(const Keyword &keyword) const {
  if (kind != Kind::KEYWORD) {
    return false;
  }

  return KEYWORD.at(data) == keyword;
}

bool Token::is_given_keyword(const Keyword &keyword_a, const Keyword &keyword_b) const {
  if (kind != Kind::KEYWORD) {
    return false;
  }

  Keyword keyword = KEYWORD.at(data);
  return keyword == keyword_a or keyword == keyword_b;
}

bool Token::is_given_literal(const Literal &literal) const {
  if (kind != Kind::LITERAL) {
    return false;
  }

  return literal == this->literal;
}

bool Token::is_given_marker(const Marker &marker) const {
  if (kind != Kind::MARKER) {
    return false;
  }

  return MARKER.at(data[0]) == marker;
}

bool Token::is_given_operator(const Operator &op) const {
  if (kind != Kind::OPERATOR) {
    return false;
  }

  return OPERATOR.at(data) == op;
}

bool Token::is_given_marker(const Marker &marker_a, const Marker &marker_b) const {
  if (kind != Kind::MARKER) {
    return false;
  }

  Marker marker = MARKER.at(data[0]);
  return marker == marker_a or marker == marker_b;
}

Marker Token::get_marker(const char character) {
  return MARKER.at(character);
}

Operator Token::get_operator(const std::string &line) {
  return OPERATOR.at(line);
}

BinaryOperator Token::get_binary_operator(const std::string &line) {
  return BINARY_OPERATOR.at(line);
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

bool Token::is_binary_operator(const std::string &line) {
  return BINARY_OPERATOR.find(line) != BINARY_OPERATOR.end();
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

Token Stream::get_next(const size_t &start_index) const {
  if (start_index + 1 >= size()) {
    throw std::runtime_error("DEV: Out of Range");
  }

  return at(start_index + 1);
}

bool Stream::is_next(const size_t start_index, Keyword keyword) const {
  if (start_index + 1 >= size()) {
    return false;
  }

  return at(start_index + 1).is_given_keyword(keyword);
}

bool Stream::is_next(const size_t start_index, std::function<bool(const Token)> predicate) const {
  if (start_index + 1 >= size()) {
    return false;
  }

  return predicate(at(start_index + 1));
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
    println("Previous Token");
    at(start_index).print();
    throw std::runtime_error("DEV: Unexepected Token");
  }

  return result;
}

Peek<Token> Stream::peek(const size_t &start_index, Token::Kind kind) const {
  return peek(start_index, [kind](const Token &token) {
    return token.is_given_kind(kind);
  });
}

Peek<Token> Stream::peek(const size_t &start_index, Marker marker) const {
  return peek(start_index, [marker](const Token &token) {
    return token.is_given_marker(marker);
  });
}

void Stream::print() const {
  for (const Token &token : *this) token.print();
}

Peek<Token> Lexer::handle_arr_literal(const std::string &line, const size_t start_index) {
  Peek<Token> result;
  
  for (size_t i = start_index + 1; i < line.size(); i++) {
    const char character = line[i];

    if (character == ']') {
      result.data.data = "[]";
      result.data.kind = Token::Kind::LITERAL;
      result.data.literal = Token::Literal::ARRAY;
      result.end_index = i;
      return result;
    }

    // TODO: Handle Array Conent
  }

  return result;
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
      result.data.literal = Token::Literal::STRING;
      result.data.data = std::move(literal);
      result.end_index = i;
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
  } else if (is_bool_literal(buffer)) {
    token.kind = Token::Kind::LITERAL;
    token.literal = Token::Literal::BOOLEAN;
    token.data = buffer;
  } else if (is_float_literal(buffer)) {
    token.kind = Token::Kind::LITERAL;
    token.literal = Token::Literal::FLOAT;
    token.data = buffer;
  } else if (is_int_literal(buffer)) {
    token.kind = Token::Kind::LITERAL;
    token.literal = Token::Literal::INTEGER;
    token.data = buffer;
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

    if (Utils::is_whitespace(character)) {
      if (buffer.empty()) continue;

      stream.push_back(handle_buffer(buffer));

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
        case Marker::LEFT_BRACKET: {
          Peek<Token> result = handle_arr_literal(line, i);
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