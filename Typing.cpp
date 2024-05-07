#pragma once

#include "Utils.h"
#include "Expression.cpp"
#include "Typing.h"

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
      return "unkown";
  }
}

void Typing::from_expression(const std::unique_ptr<Expression> &expression) {
  data = expression->literal;

  if (expression->literal == Token::Literal::STRUCT) {
    Object *object = static_cast<Object *>(expression.get());
    this->value = object->name;
  // for some reason
  // this triggers a bad alloc error when the array has init or len binary expressions
  } else if (expression->literal == Token::Literal::ARRAY) {
    Array *array = static_cast<Array *>(expression.get());
    try {
      this->value = array->typing.value;
      this->data = array->typing.data;
      this->children = array->typing.children;
    } catch (std::bad_alloc &e) {
      std::cerr << "bad alloc caught: " << e.what() << std::endl;
    }
  } else if (expression->literal != Token::Literal::ARRAY) {
    this->value = expression->value;
  }   
}

Peek<Typing> Typing::build(Stream &stream, const size_t &start_index) {
  size_t index = start_index;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_kind(Token::Kind::IDENTIFIER) ||
        token.is_given_literal(Token::Literal::ARRAY);
    });

    if (next.data.is_given_kind(Token::Kind::IDENTIFIER)) {
      Peek<Typing> result;
      
      result.data.value = next.data.data;
      result.end_index = next.end_index;
      return result;
    } else {
      Peek<Typing> child = Typing::build(stream, next.end_index);
      Peek<Typing> result;

      result.data.data = Token::Literal::ARRAY;
      result.data.value = next.data.data + child.data.value;
      result.data.children.push_back(child.data);
      result.end_index = child.end_index;
      return result;
    }
  }
  
  throw std::runtime_error("Invalid Type");
}

void Typing::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);

  println(indentation + "Typing {");
  
  if (data == Token::Literal::ARRAY) {
    println(indentation + "  kind: Array");
  } else if (data == Token::Literal::STRUCT) {
    println(indentation + "  kind: Struct");
  } else if (data == Token::Literal::UNKNOWN) {
    println(indentation + "  kind: Unknown");
  } else {
    println(indentation + "  kind: " + "built-in type");
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
  
  if (data == Token::Literal::ARRAY) {
    result += indentation + "  kind: Array\n";
  } else if (data == Token::Literal::STRUCT) {
    result += indentation + "  kind: Struct\n";
  } else if (data == Token::Literal::UNKNOWN) {
    result += indentation + "  kind: Unknown\n";
  } else {
    result += indentation + "  kind: " + "built-in\n";
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