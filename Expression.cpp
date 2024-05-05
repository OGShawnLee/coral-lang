#pragma once

#include "Utils.h"
#include "Expression.h"
#include "Array.cpp"
#include "Function.cpp"
#include "Struct.cpp"

class Variable;

std::map<Expression::Type, std::string> EXPRESSION_TYPE_NAME = {
  {Expression::Type::LITERAL, "Literal"},
  {Expression::Type::IDENTIFIER, "Identifier"},
  {Expression::Type::BINARY, ""},
  {Expression::Type::ASSIGNMENT, "Assignment"},
  {Expression::Type::FUNCTION_CALL, "Function Call"},
  {Expression::Type::PROPERTY_ACCESS, "Property Access"},
};

bool Expression::is_expression(Stream &stream, const size_t &start_index) {
  return 
    stream.is_next(start_index, [](const Token token) {
      return token.is_given_kind(Token::Kind::IDENTIFIER, Token::Kind::LITERAL);
    }) || 
    BinaryExpression::is_binary_expression(stream, start_index);
}

PeekPtr<Expression> Expression::build(
  Stream &stream, 
  const size_t &start_index,
  const bool &with_binary
) {
  PeekPtr<Expression> result;

  if (not is_expression(stream, start_index)) {
    throw std::runtime_error("DEV: Not an Expression");
  }

  if (BinaryExpression::is_binary_expression(stream, start_index) && with_binary) {
    PeekPtr<BinaryExpression> child = BinaryExpression::build(stream, start_index);
    result.data->value = child.data->value;
    result.data = std::move(child.data);
    result.end_index = child.end_index;
  } else {
    bool is_arr_literal = Array::is_arr_literal(stream, start_index);
    bool is_fn_call = Function::is_fn_call(stream, start_index);
    bool is_struct_literal = Struct::is_struct_literal(stream, start_index);

    if (is_arr_literal || is_fn_call || is_struct_literal) {
      if (is_arr_literal) {
        PeekPtr<Array> child = Array::build(stream, start_index);

        result.data->value = child.data->value;
        result.data = std::move(child.data);
        result.end_index = child.end_index;
      } else if (is_struct_literal) {
        PeekPtr<Object> child = Struct::build_as_struct_literal(stream, start_index);

        result.data->value = child.data->value;
        result.data = std::move(child.data);
        result.end_index = child.end_index;
      } else {
        PeekPtr<Expression> child = Function::build_as_fn_call(stream, start_index);

        result.data->value = child.data->value;
        result.data = std::move(child.data);
        result.end_index = child.end_index;
      }

      // <fn call/struct literal> <operator> <expression>
      bool is_incomplete = stream.is_next(result.end_index, [](const Token token) {
        return Token::is_binary_operator(token.data);
      });

      if (is_incomplete) {
        PeekPtr<BinaryExpression> binary = BinaryExpression::build(stream, result.end_index, false);

        binary.data->left = std::move(result.data);
        result.data = std::move(binary.data);
        result.end_index = binary.end_index;
      }

      return result;
    }

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

  if (type == Type::FUNCTION_CALL) {
    println(indentation + "Function Call {");
    println(indentation + "  name: " + value);
    
    if (not arguments.empty()) {
      println(indentation + "  arguments: [");
      for (const std::unique_ptr<Expression> &argument : arguments) {
        argument->print(indent + 2);
      }
      println(indentation + "  ]");
    }

    println(indentation + "}");
    return;
  }

  println(indentation + "Expression {");
  println(indentation + "  value: " + value);
  println(indentation + "}");
}

std::string Expression::to_string(size_t indent) const {
  if (type == Type::FUNCTION_CALL) {
    std::string indentation = Utils::get_indent(indent);
    std::string result = "Function Call {\n";

    result += indentation + "  name: " + value + "\n";
    
    if (not arguments.empty()) {
      result += indentation + "  arguments: [\n";
      for (const std::unique_ptr<Expression> &argument : arguments) {
        result += indentation + "    " + argument->to_string(indent + 2) + "\n";
      }
      result += indentation + "  ]\n";
    }

    result += indentation + "}";
    return result;
  }

  return EXPRESSION_TYPE_NAME.at(type) + " { " + value + " }";
}

bool BinaryExpression::is_binary_expression(Stream &stream, const size_t &start_index) {
  return 
  Expression::is_expression(stream, start_index) &&
  stream.is_next(start_index + 1, [](const Token token) {
    return Token::is_binary_operator(token.data);
  });
}

PeekPtr<BinaryExpression> BinaryExpression::build(
  Stream &stream, 
  const size_t &start_index,
  const bool &with_left
) {
  PeekPtr<BinaryExpression> result;

  if (with_left) {
    PeekPtr<Expression> left = Expression::build(stream, start_index, false);
    result.data->value = left.data->value;
    result.data->left = std::move(left.data);
    result.end_index = left.end_index;
  }

  Peek<Token> operation = stream.peek(start_index + with_left, [](const Token token) {
    return Token::is_binary_operator(token.data);
  });

  if (operation.data.data == "=") {
    result.data->type = Expression::Type::ASSIGNMENT;
  } else if (operation.data.data == ":") {
    result.data->type = Expression::Type::PROPERTY_ACCESS;
  } else {
    result.data->type = Expression::Type::BINARY;
  }

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
  
  if (type != Expression::Type::BINARY) {
    std::string name = EXPRESSION_TYPE_NAME.at(type);
    println(indentation + "  type: " + name);
  }  

  println(indentation + "  left: " + left->to_string(indent + 1));
  println(indentation + "  operation: " + operation);
  println(indentation + "  right: " + right->to_string(indent + 1));
  println(indentation + "}");
}

std::string BinaryExpression::to_string(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  std::string result;

  result += "Binary Expression {\n";
  
  if (type != Expression::Type::BINARY) {
    std::string name = EXPRESSION_TYPE_NAME.at(type);
    result += indentation + "  type: " + name + "\n";
  }

  result += indentation + "  left: " + left->to_string(indent + 1) + "\n";
  result += indentation + "  operation: " + operation + "\n";
  result += indentation + "  right: " + right->to_string(indent + 1) + "\n";
  result += indentation + "}";

  return result;
}
