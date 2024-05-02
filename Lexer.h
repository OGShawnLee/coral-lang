#pragma once

#include <vector>
#include <string>
#include <map>

enum class Operator {
  ASSIGN,
};

enum class Keyword {
  FN,
  VAR,
  VAL,
};

enum class Marker {
  STR_QUOTE,
};

class Token {
  public:
    enum class Kind {
      IDENTIFIER,
      LITERAL,
      MARKER,
      OPERATOR,
      KEYWORD,
    };

    std::string data;
    Kind kind;

    Token() = default;

    explicit Token(const char character);

    static Marker get_marker(const char character);

    static Operator get_operator(const std::string &buffer);

    static Keyword get_keyword(const std::string &buffer);

    static bool is_operator(const std::string &buffer);

    static bool is_keyword(const std::string &buffer);

    static bool is_marker(const char character);

    void print() const;
};

typedef std::vector<Token> Stream;

struct Result {
  Token data;
  size_t end_index;
};

class Lexer {
  static Token handle_buffer(std::string &buffer);

  static Result handle_str_literal(const std::string &line, const size_t start_index);

  public:
    static Stream lex_ln(const std::string &line);
    
    static Stream lex_file(const std::string &line);
};
