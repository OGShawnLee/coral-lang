#pragma once

#include "Utils.h"
#include "Function.h"
#include "Parser.h"

Function::Function() {
  type = Type::FUNCTION_DECLARATION;
}

bool Function::is_fn_call(Stream &stream, const size_t &start_index) {
  return 
    stream.is_next(start_index, [](const Token &token) {
      return token.is_given_kind(Token::Kind::IDENTIFIER);
    }) && 
    stream.is_next(start_index + 1, [](const Token &token) {
      return token.is_given_marker(Marker::LEFT_PARENTHESIS);
    });
}

bool Function::is_lambda(Stream &stream, const size_t &start_index) {
  return
    stream.is_next(start_index, [](const Token &token) {
      return token.is_given_keyword(Keyword::FUNCTION);
    }) &&
    stream.is_next(start_index + 1, [](const Token &token) {
      return token.is_given_marker(Marker::LEFT_PARENTHESIS, Marker::LEFT_BRACE);
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

Peek<Typing> Function::build_as_fn_type(Stream &stream, const size_t &start_index) {
  if (not stream.at(start_index).is_given_keyword(Keyword::FUNCTION)) {
    throw std::runtime_error("DEV: Expected 'fn' keyword");
  }

  // fn (<name> fn) { <body> }
  // fn (<name> fn (...)) { <body> }
  // fn (<name> fn, ...) { <body> }
  Peek<Token> opening = stream.peek(start_index, [](const Token &token) {
    return token.is_given_marker(
      Marker::LEFT_PARENTHESIS, 
      Marker::RIGHT_PARENTHESIS,
      Marker::COMMA
    );
  });

  Peek<Typing> result;

  if (not opening.data.is_given_marker(Marker::LEFT_PARENTHESIS)) {
    result.data.data = Token::Literal::LAMBDA;
    result.data.value = "fn";
    result.end_index = start_index;
    return result;
  }

  result.data.value = "fn (";

  size_t index = opening.end_index;
  // For Storing Each Parameter Value
  // and then we concatenate them to the result value
  std::vector<std::string> values;

  while (index < stream.size()) {
    Peek<Token> next = stream.peek(index, [](const Token &token) {
      return 
        token.is_given_marker(Marker::RIGHT_PARENTHESIS, Marker::COMMA) || 
        token.is_given_kind(Token::Kind::IDENTIFIER) ||
        token.is_given_keyword(Keyword::FUNCTION);
    });

    if (next.data.is_given_marker(Marker::COMMA)) {
      index = next.end_index;
      continue;
    }

    if (next.data.is_given_marker(Marker::RIGHT_PARENTHESIS)) {
      result.data.data = Token::Literal::LAMBDA;
      result.data.value += Utils::join(values, ", ");
      result.data.value += ")";
      result.end_index = next.end_index;
      return result;
    }

    Peek<Typing> parameter = Typing::build(stream, index);
    values.push_back(parameter.data.value);
    result.data.children.push_back(std::move(parameter.data));
    index = parameter.end_index;
  }

  throw std::runtime_error("DEV: Unterminated Function Type");
}

PeekPtr<Lambda> Function::build_as_lambda(Stream &stream, const size_t &start_index) {
  PeekPtr<Lambda> result;

  Token keyword = stream.get_next(start_index);

  if (not keyword.is_given_keyword(Keyword::FUNCTION)) {
    throw std::runtime_error("DEV: Expected 'fn' keyword");
  }

  Peek<Token> opening = stream.peek(start_index + 1, [](const Token &token) {
    return token.is_given_marker(Marker::LEFT_PARENTHESIS, Marker::LEFT_BRACE);
  });

  if (opening.data.is_given_marker(Marker::LEFT_BRACE)) {
    PeekVectorPtr<Statement> body = Parser::build_block(stream, opening.end_index);
    
    result.data->variant = Lambda::Variant::LITERAL;
    result.data->literal = Token::Literal::LAMBDA;
    result.data->children = std::move(body.data);
    result.end_index = body.end_index;
    return result;
  }

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

  PeekVectorPtr<Statement> body = Parser::build_block(stream, index + 1);

  result.data->variant = Lambda::Variant::LITERAL;
  result.data->literal = Token::Literal::LAMBDA;
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
      result.data->kind = Kind::EXPRESSION;
      result.data->variant = Expression::Variant::FUNCTION_CALL;
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

void Lambda::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "Lambda {");
  
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

std::string Lambda::to_string(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  std::string result = "Lambda {\n";

  if (not parameters.empty()) {
    result += indentation + "  parameters: [\n";

    for (const std::unique_ptr<Variable> &parameter : parameters) {
      if (parameter->value) {
        result += indentation + "    " + parameter->name + ": ";
        result += parameter->value->to_string(indent + 2) + "\n";
      } else {
        result += indentation + "    " + parameter->name + ": " + parameter->typing.to_string(indent + 2) + "\n";
      }
    }

    result += indentation + "  ]\n";
  }

  if (not children.empty()) {
    result += indentation + "  body: []\n";
  }

  result += indentation + "}";
  return result;
}
