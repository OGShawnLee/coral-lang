#pragma once

#include "Utils.h"
#include "Expression.h"

std::map<Expression::Type, std::string> EXPRESSION_TYPE_NAME = {
  {Expression::Type::LITERAL, "Literal"},
  {Expression::Type::IDENTIFIER, "Identifier"},
};

bool Expression::is_expression(Stream &stream, const size_t &start_index) {
  return 
    stream.is_next(start_index, [](const Token token) {
      return token.is_given_kind(Token::Kind::IDENTIFIER, Token::Kind::LITERAL);
    }) || 
     BinaryExpression::is_binary_expression(stream, start_index);
}

PeekPtr<Expression> Expression::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Expression> result;

  if (not is_expression(stream, start_index)) {
    throw std::runtime_error("DEV: Not an Expression");
  }

  if  (BinaryExpression::is_binary_expression(stream, start_index)) {
    PeekPtr<BinaryExpression> child = BinaryExpression::build(stream, start_index);
    result.data->value = child.data->value;
    result.data = std::move(child.data);
    result.end_index = child.end_index;
  } else {
    Token next = stream.get_next(start_index);

    result.data->type = 
      next.kind == Token::Kind::IDENTIFIER ? Type::IDENTIFIER : Type::LITERAL;

    result.data->value = next.data;
    result.end_index = start_index + 1;
  }

  return result;
}

void Expression::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);

  println(indentation + "Expression {");
  println(indentation + "  value: " + value);
  println(indentation + "}");
}

std::string Expression::to_string(size_t indent) const {
  return EXPRESSION_TYPE_NAME.at(type) + " { " + value + " }";
}

bool BinaryExpression::is_binary_expression(Stream &stream, const size_t &start_index) {
  return 
  Expression::is_expression(stream, start_index) &&
  stream.is_next(start_index + 1, [](const Token token) {
    return Token::is_binary_operator(token.data);
  });
}

PeekPtr<BinaryExpression> BinaryExpression::build(Stream &stream, const size_t &start_index) {
  PeekPtr<BinaryExpression> result;

  Token next = stream.get_next(start_index);
  Token::Kind kind = next.kind;

  result.data->left = std::make_unique<Expression>();
  result.data->left->value = result.data->value = next.data;

  if (kind == Token::Kind::IDENTIFIER) {
    result.data->left->type = Expression::Type::IDENTIFIER;
  } else if (kind == Token::Kind::LITERAL) {
    result.data->left->type = Expression::Type::LITERAL;
  } else {
    result.data->left = std::make_unique<Expression>();
    println("Unexpected Token: ");
    next.print();
    throw std::runtime_error("DEV: Invalid Expression");
  }

  Peek<Token> operation = stream.peek(start_index + 1, [](const Token token) {
    return Token::is_binary_operator(token.data);
  });

  result.data->operation = operation.data.data;
  result.data->value += operation.data.data;
  result.end_index = operation.end_index;

  PeekPtr<Expression> right = Expression::build(stream, operation.end_index);
  result.data->value += " " + right.data->value;
  result.data->right = std::move(right.data);
  result.end_index = right.end_index;

  return result;
}

void BinaryExpression::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);

  println(indentation + "Binary Expression {");
  println(indentation + "  left: " + left->to_string(indent + 1));
  println(indentation + "  operation: " + operation);
  println(indentation + "  right: " + right->to_string(indent + 1));
  println(indentation + "}");
}

std::string BinaryExpression::to_string(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  std::string result;

  result += "Binary Expression {\n";
  result += indentation + "  left: " + left->to_string(indent + 1) + "\n";
  result += indentation + "  operation: " + operation + "\n";
  result += indentation + "  right: " + right->to_string(indent + 1) + "\n";
  result += indentation + "}";

  return result;
}