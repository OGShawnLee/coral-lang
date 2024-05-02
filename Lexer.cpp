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
};

std::map<std::string, Keyword> KEYWORD = {
  {"fn", Keyword::FN},
  {"var", Keyword::VAR},
  {"val", Keyword::VAL},
};

std::map<char, Marker> MARKER = {
  {'"', Marker::STR_QUOTE},
};

Token::Token(const char character) {
  if (not is_marker(character)) {
    throw std::runtime_error("DEV: Not a Marker");
  }

  kind = Kind::MARKER;
  data = character;
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

bool Token::is_operator(const std::string &line) {
  return OPERATOR.find(line) != OPERATOR.end();
}

bool Token::is_keyword(const std::string &line) {
  return KEYWORD.find(line) != KEYWORD.end();
}

bool Token::is_marker(const char character) {
  return MARKER.find(character) != MARKER.end();
}

void Token::print() const {
  std::string kind = get_kind_name(this->kind);
  println(kind + " { data: " + data + " }");
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
  }

  throw std::runtime_error("DEV: Unterminated string literal");
}

Token Lexer::handle_buffer(std::string &buffer) {
  Token token;

  if (Token::is_operator(buffer)) {
    token.kind = Token::Kind::OPERATOR;
  } else if (Token::is_keyword(buffer)) {
    token.kind = Token::Kind::KEYWORD;
  } else {
    token.kind = Token::Kind::IDENTIFIER;
  }

  token.data = std::move(buffer);
  buffer.clear();

  return token;
}

Stream Lexer::lex_ln(const std::string &line) {
  Stream stream;
  std::string buffer;
  
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