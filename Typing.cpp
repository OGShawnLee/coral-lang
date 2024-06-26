#pragma once

#include "Utils.h"
#include "Expression.cpp"
#include "Typing.h"
#include "Function.cpp"

Token::Literal Typing::infer_built_in_type_from_string(std::string data) {
  if (data == "bool") {
    return Token::Literal::BOOLEAN;
  } else if (data == "int") {
    return Token::Literal::INTEGER;
  } else if (data == "str") {
    return Token::Literal::STRING;
  } else if (data == "float") {
    return Token::Literal::FLOAT;
  } else if (isupper(data[0])) {
    return Token::Literal::STRUCT;
  } else {
    return Token::Literal::UNKNOWN;
  }
}

std::string Typing::infer_built_in_type(Token::Literal literal) {
  switch (literal) {
    case Token::Literal::ARRAY:
      return "[]";
    case Token::Literal::BOOLEAN:
      return "bool";
    case Token::Literal::INTEGER:
      return "int";
    case Token::Literal::STRING:
      return "str";
    case Token::Literal::FLOAT:
      return "float";
    default:
      return "unknown";
  }
}

void Typing::from_expression(const std::unique_ptr<Expression> &expression) {
  if (expression->variant == Expression::Variant::BLOCK) {
    Block *block = static_cast<Block *>(expression.get());
    this->data = block->typing.data;
    this->value = block->typing.value;
    this->children = block->typing.children;
    return;
  }

  if (expression->variant != Expression::Variant::LITERAL) {
    this->data = Token::Literal::UNKNOWN;
    this->value = infer_built_in_type(Token::Literal::UNKNOWN);
    return;
  } 

  switch (data = expression->literal) {
    case Token::Literal::ARRAY: {
      Array *array = static_cast<Array *>(expression.get());
      this->value = array->typing.value;
      this->data = array->typing.data;
      this->children = array->typing.children;
    } break;
    case Token::Literal::LAMBDA: {
      this->value = "fn";
    } break;
    case Token::Literal::STRUCT: {
      Object *object = static_cast<Object *>(expression.get());
      this->value = object->name;
    } break;
    default: {
      this->value = infer_built_in_type(data);
    } break;
  }
}

Typing Typing::create(const Token::Literal &literal) {
  Typing typing;
  typing.data = literal;
  typing.value = infer_built_in_type(literal);
  return typing;
}

Peek<Typing> Typing::build(Stream &stream, const size_t &start_index) {
  size_t index = start_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_kind(Token::Kind::IDENTIFIER) ||
        token.is_given_literal(Token::Literal::ARRAY) ||
        token.is_given_keyword(Keyword::FUNCTION);
    });

    Peek<Typing> result;
    
    if (next.data.kind == Token::Kind::IDENTIFIER) {
      result.data.data = infer_built_in_type_from_string(next.data.data);
      result.data.value = next.data.data;
      result.end_index = next.end_index;
      return result;
    } 

    if (next.data.kind == Token::Kind::LITERAL) {
      Peek<Typing> child = Typing::build(stream, next.end_index);

      result.data.data = Token::Literal::ARRAY;
      result.data.value = next.data.data + child.data.value;
      result.data.children.push_back(child.data);
      result.end_index = child.end_index;
      return result;
    }

    return Function::build_as_fn_type(stream, next.end_index);
  }
  
  throw std::runtime_error("Invalid Type");
}

void Typing::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);

  println(indentation + "Typing {");

  switch (data) {
    case Token::Literal::ARRAY:
      println(indentation + "  kind: Array");
      break;
    case Token::Literal::STRUCT:
      println(indentation + "  kind: Struct");
      break;
    case Token::Literal::LAMBDA:
      println(indentation + "  kind: Function");
      break;
    case Token::Literal::UNKNOWN:
      println(indentation + "  kind: Unknown");
      break;
    default:
      println(indentation + "  kind: built-in");
      break;
  }

  println(indentation + "  value: " + value);
  
  if (not children.empty()) {
    println(indentation + "  children: [");
    
    for (const Typing &child : children) {
      child.print(indent + 2);
    }
    
    println(indentation + "  ]");
  }

  println(indentation + "}"); 
}

std::string Typing::to_string(size_t indent, const bool &is_child) const {
  std::string indentation = Utils::get_indent(indent);
  std::string result = is_child ? indentation + "Typing {\n" : "{\n";
  
  switch (data) {
    case Token::Literal::ARRAY:
      result += indentation + "  kind: Array\n";
      break;
    case Token::Literal::STRUCT:
      result += indentation + "  kind: Struct\n";
      break;
    case Token::Literal::LAMBDA:
      result += indentation + "  kind: Function\n";
      break;
    case Token::Literal::UNKNOWN:
      result += indentation + "  kind: Unknown\n";
      break;
    default:
      result += indentation + "  kind: built-in\n";
      break;
  }

  result += indentation + "  value: " + value + "\n";

  if (not children.empty()) {
    result += indentation + "  children: [\n";
    
    for (const Typing &child : children) {
      result += child.to_string(indent + 2, true);
    }
    
    result += indentation + "  ]\n";
  }

  result += indentation + "}\n";
  return result;
}