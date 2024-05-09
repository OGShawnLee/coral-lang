#pragma once

#include "Utils.h"
#include <functional>
#include <vector>
#include <string>
#include <map>

enum class Operator {
  ASSIGN,
  ADDITION,
  ASSIGN_ADDITION,
  SUBTRACTION,
  ASSIGN_SUBTRACTION,
  MULTIPLICATION,
  ASSIGN_MULTIPLICATION,
  DIVISION,
  ASSIGN_DIVISION,
  MODULUS,
  ASSIGN_MODULUS,
  AND,
  OR,
  NOT,
  NOT_CHAR,
  MORE_THAN,
  MORE_THAN_EQUAL,
  LESS_THAN,
  LESS_THAN_EQUAL,
  EQUAL,
  NOT_EQUAL,
};

enum class BinaryOperator {
  ASSIGN,
  ADDITION,
  ASSIGN_ADDITION,
  SUBTRACTION,
  ASSIGN_SUBTRACTION,
  MULTIPLICATION,
  ASSIGN_MULTIPLICATION,
  DIVISION,
  ASSIGN_DIVISION,
  MODULUS,
  ASSIGN_MODULUS,
  AND,
  OR,
  COLON,
  MORE_THAN,
  MORE_THAN_EQUAL,
  LESS_THAN,
  LESS_THAN_EQUAL,
  EQUAL,
  NOT_EQUAL,
};

enum class Keyword {
  VAR,
  VAL,
  ENUM,
  STRUCT,
  FUNCTION,
  RETURN,
  FOR,
  IN,
  IF,
  ELSE,
  CONTINUE,
  BREAK,
  MATCH,
  WHEN,
};

enum class Marker {
  STR_QUOTE,
  STR_INJECTION,
  RIGHT_BRACE,
  LEFT_BRACE,
  RIGHT_PARENTHESIS,
  LEFT_PARENTHESIS,
  RIGHT_BRACKET,
  LEFT_BRACKET,
  COMMA,
  COLON,
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

    /*
      Array, Boolean, Integer, String, Float are literals determined by the Lexer 
      The rest are assigned by the Parser and are stored here for Typing class convienence
    */
    enum class Literal {
      ARRAY,
      BOOLEAN,
      INTEGER,
      STRING,
      FLOAT,
      // Assigned by the Parser
      LAMBDA,
      VOID,
      STRUCT,
      UNKNOWN,
    };

    std::string data;
    Kind kind;
    Literal literal;
    std::vector<std::string> injections;

    Token() = default;

    explicit Token(const char character);

    bool is_given_kind(const Kind &kind) const;
    bool is_given_kind(const Kind &kind_a, const Kind &kind_b) const;
    bool is_given_kind(const Kind &kind_a, const Kind &kind_b, const Kind &kind_c) const;

    bool is_given_literal(const Literal &literal) const;

    bool is_given_keyword(const Keyword &keyword) const;
    bool is_given_keyword(const Keyword &keyword_a, const Keyword &keyword_b) const;

    bool is_given_marker(const Marker &marker) const;
    bool is_given_marker(const Marker &marker_a, const Marker &marker_b) const;
    bool is_given_marker(const Marker &marker_a, const Marker &marker_b, const Marker &marker_c) const;

    bool is_given_operator(const Operator &op) const;

    static Marker get_marker(const char character);

    static Operator get_operator(const std::string &buffer);
    static BinaryOperator get_binary_operator(const std::string &buffer);

    static Keyword get_keyword(const std::string &buffer);

    static bool is_bool_literal(const std::string &buffer);

    static bool is_float_literal(const std::string &buffer);

    static bool is_int_literal(const std::string &buffer);

    static bool is_operator(const std::string &buffer);
    static bool is_operator(const char &character);

    static bool is_binary_operator(const std::string &buffer);

    static bool is_keyword(const std::string &buffer);

    static bool is_marker(const char character);

    static bool is_valid_id_char(const char character);

    static bool is_next(const std::string &line, const size_t start_index, std::function<bool(const char)> predicate);

    void print() const;
};

class Stream : public std::vector<Token> {
  public:
    Stream() = default;

    Token get_next(const size_t &start_index) const;

    bool is_next(const size_t start_index, std::function<bool(const Token)> predicate) const;
    bool is_next(const size_t start_index, Keyword keyword) const;

    Peek<Token> peek(const size_t &start_index, const std::function<bool(const Token)> predicate) const;
    Peek<Token> peek(const size_t &start_index, Token::Kind kind) const;
    Peek<Token> peek(const size_t &start_index, Marker marker) const;

    void print() const;
};

struct Result {
  Token data;
  size_t end_index;
};

class Lexer {
  static Token handle_buffer(std::string &buffer);

  static Peek<Token> handle_arr_literal(const std::string &line, const size_t start_index);

  static Peek<std::string> handle_str_injection(const std::string &line, const size_t start_index);

  static Result handle_str_literal(const std::string &line, const size_t start_index);

  public:
    static Stream lex_ln(std::string line);
    
    static Stream lex_file(const std::string &line);
};
