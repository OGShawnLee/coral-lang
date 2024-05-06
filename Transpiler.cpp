#pragma once

#include "Transpiler.h"
#include "Parser.cpp"

std::map<std::string, std::string> BUILT_IN_FN = {
  {"println", "print"},
  {"readln", "input"},
};

std::string get_built_in_fn(const std::string &name) {
  return BUILT_IN_FN.at(name);
}

bool is_built_in_fn(const std::string &name) {
  return BUILT_IN_FN.find(name) != BUILT_IN_FN.end();
}


std::string Transpiler::get_value(const std::unique_ptr<Statement> &statement) {
  if (statement->kind != Statement::Kind::EXPRESSION) {
    throw std::runtime_error("DEV: Expected Expression");
  }

  auto element = static_cast<Expression *>(statement.get());

  return get_value(element);
}

std::string Transpiler::get_value(const std::unique_ptr<Expression> &expression) {
  return get_value(expression.get());
}

std::string Transpiler::get_value(const Expression *expression) {
  std::string output;

  switch (expression->variant) {
    case Expression::Variant::IDENTIFIER: {
      output = expression->value;
      break;
    }
    case Expression::Variant::LITERAL: {
      if (expression->literal == Token::Literal::STRING) {
        output = "\"" + expression->value + "\"";
      } else if (expression->literal == Token::Literal::BOOLEAN) {
        output = Utils::capitalise(expression->value);
      } else {
        output = expression->value;
      }
      break;
    }
    case Expression::Variant::FUNCTION_CALL: {
      if (is_built_in_fn(expression->value)) {
        output += get_built_in_fn(expression->value) + "(";
      } else {
        output += expression->value + "(";
      }

      for (size_t i = 0; i < expression->arguments.size(); i++) {
        output += get_value(expression->arguments[i]);
        if (i < expression->arguments.size() - 1) {
          output += ", ";
        }
      }
      output += ")";
      break;
    }
    default:
      println("Expression Unsupported");
  }

  return output;
}

void Transpiler::handle_statement(const std::unique_ptr<Statement> &statement) {
  switch (statement->type) {
    case Statement::Type::VARIABLE_DECLARATION: {
      std::string indent = Utils::get_indent(indentation); 
      auto variable = static_cast<Variable *>(statement.get());
      output += indent + variable->name + " = " + get_value(variable->value) + "\n";
      break;
    }
    default:      
      println("Statement Unsupported");
  }
}

void Transpiler::transpile(const std::string &file_path, const std::string &output_path) {
  Statement stream = Parser::parse(file_path);

  for (const auto &statement : stream.children) {
    switch (statement->kind) {
      case Statement::Kind::STATEMENT: {
        handle_statement(statement);
        break;
      }
      case Statement::Kind::EXPRESSION: {
        output += get_value(statement) + "\n";
        break;
      }
      default:
        println("Kind Unsupported");
    }
  }

  Utils::write_file(output_path, output);
}