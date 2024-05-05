#pragma once

#include "Utils.h"
#include "Function.h"
#include "Parser.h"

bool Function::is_fn_call(Stream &stream, const size_t &start_index) {
  return 
    stream.is_next(start_index, [](const Token &token) {
      return token.is_given_kind(Token::Kind::IDENTIFIER);
    }) && 
    stream.is_next(start_index + 1, [](const Token &token) {
      return token.is_given_marker(Marker::LEFT_PARENTHESIS);
    });
}

PeekPtr<Function> Function::build(Stream &stream, const size_t &start_index) {
  PeekPtr<Function> result;

  Token keyword = stream[start_index];
  if (not keyword.is_given_keyword(Keyword::FUNCTION)) {
    throw std::runtime_error("DEV: Expected 'fn' keyword");
  }

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  Peek<Token> opening = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_PARENTHESIS, Marker::LEFT_BRACE);
  });

  // fn <name> { <body> }
  if (opening.data.is_given_marker(Marker::LEFT_BRACE)) {
    PeekVectorPtr<Statement> body = Parser::build_block(stream, opening.end_index);
    
    result.data->name = name.data.data;
    result.data->children = std::move(body.data);
    result.end_index = body.end_index;
    return result;
  }

  // fn <name> ( <parameters> ) { <body> }
  size_t index = opening.end_index;

  // Parsing Parameters
  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_marker(Marker::RIGHT_PARENTHESIS, Marker::COMMA) || 
        token.is_given_kind(Token::Kind::IDENTIFIER);
    });

    if (next.data.is_given_marker(Marker::COMMA)) {
      index = next.end_index;
      continue;
    }

    if (next.data.is_given_marker(Marker::RIGHT_PARENTHESIS)) {
      index = next.end_index;
      break;
    }
    
    PeekPtr<Variable> parameter = Variable::build_as_field(stream, index);
    result.data->parameters.push_back(std::move(parameter.data));
    index = parameter.end_index;
  }

  opening = stream.peek(index, Marker::LEFT_BRACE);

  PeekVectorPtr<Statement> body = Parser::build_block(stream, opening.end_index);

  result.data->name = name.data.data;
  result.data->children = std::move(body.data);
  result.end_index = body.end_index;
  return result;
}

void Function::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Function {");
  println(indentation + "  name: " + name);
  
  if (not parameters.empty()) {
    println(indentation + "  parameters: [");

    for (const std::unique_ptr<Variable> &parameter : parameters) {
      parameter->print(indent + 2);
    }

    println(indentation + "  ]");
  }

  if (not children.empty()) {
    println(indentation + "  body: [");

    for (const std::unique_ptr<Statement> &child : children) {
      child->print(indent + 2);
    }

    println(indentation + "  ]");
  }

  println(indentation + "}");
}

PeekPtr<Expression> Function::build_as_fn_call(Stream &stream, const size_t &start_index) {
  PeekPtr<Expression> result;

  Peek<Token> name = stream.peek(start_index, [](const Token &token) {
    return token.is_given_kind(Token::Kind::IDENTIFIER);
  });

  result.data->value += name.data.data;

  Peek<Token> opening = stream.peek(name.end_index, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_PARENTHESIS);
  });

  size_t index = opening.end_index;

  while (index < stream.size()) {
    Token next = stream.get_next(index);

    if (next.is_given_marker(Marker::COMMA)) {
      index++;
      continue;
    }

    if (next.is_given_marker(Marker::RIGHT_PARENTHESIS)) {
      result.data->type = Expression::Type::FUNCTION_CALL;
      result.end_index = index + 1;
      return result;
    }

    if (Expression::is_expression(stream, index)) {
      PeekPtr<Expression> argument = Expression::build(stream, index);
      result.data->arguments.push_back(std::move(argument.data));
      index = argument.end_index;
    }
  }

  throw std::runtime_error("USER: Unterminated Function Call " + name.data.data);
}