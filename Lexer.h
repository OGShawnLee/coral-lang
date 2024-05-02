#pragma once

#include <functional>
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
  STR_INJECTION,
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
    std::vector<std::string> injections;

    Token() = default;

    explicit Token(const char character);

    static Marker get_marker(const char character);

    static Operator get_operator(const std::string &buffer);

    static Keyword get_keyword(const std::string &buffer);

    static bool is_operator(const std::string &buffer);

    static bool is_keyword(const std::string &buffer);

    static bool is_marker(const char character);

    static bool is_valid_id_char(const char character);

    static bool is_next(const std::string &line, const size_t start_index, std::function<bool(const char)> predicate);

    void print() const;
};

typedef std::vector<Token> Stream;

template <typename T>
struct Peek {
  T data;
  size_t end_index;
};

struct Result {
  Token data;
  size_t end_index;
};

class Lexer {
  static Token handle_buffer(std::string &buffer);

  static Peek<std::string> handle_str_injection(const std::string &line, const size_t start_index);

  static Result handle_str_literal(const std::string &line, const size_t start_index);

  public:
    static Stream lex_ln(std::string line);
    
    static Stream lex_file(const std::string &line);
};
