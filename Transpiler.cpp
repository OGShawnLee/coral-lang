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

std::string handle_str_literal(const Expression* literal) {
  const auto str = static_cast<const String*>(literal);

  if (str->injections.empty()) {
    return "\"" + str->value + "\"";
  
  } else {
    std::string output = "f\"" + str->value + "\"";
    for (const auto &injection : str->injections) {
      Utils::replace(output, "#" + injection, '{' + injection + '}');
    }
    return output;
  }
}

std::string Transpiler::handle_expression(
  const std::unique_ptr<Statement> &statement,
  const size_t &indentation
) {
  if (statement->kind != Statement::Kind::EXPRESSION) {
    throw std::runtime_error("DEV: Expected Expression");
  }

  auto element = static_cast<Expression *>(statement.get());

  return handle_expression(element, indentation);
}

std::string Transpiler::handle_expression(
  const std::unique_ptr<Expression> &expression,
  const size_t &indentation
) {
  return handle_expression(expression.get(), indentation);
}

std::string Transpiler::handle_expression(
  const Expression *expression,
  const size_t &indentation
) {
  std::string output;

  switch (expression->variant) {
    case Expression::Variant::ASSIGNMENT:
    case Expression::Variant::PROPERTY_ACCESS:
    case Expression::Variant::BINARY: {
      auto binary = static_cast<const BinaryExpression*>(expression);
      std::string indent = Utils::get_indent(indentation);
      std::string left = handle_expression(binary->left);
      std::string right = handle_expression(binary->right);
      
      if (expression->variant != Expression::Variant::PROPERTY_ACCESS) {
        output += indent + left + " " + binary->operation + " " + right;
      } else output += indent + left + "." + right;
      
      break;
    }
    case Expression::Variant::IDENTIFIER: {
      output = expression->value;
      break;
    }
    case Expression::Variant::LITERAL: {
      if (expression->literal == Token::Literal::STRING) {
        output = handle_str_literal(expression);
      } else if (expression->literal == Token::Literal::BOOLEAN) {
        output = Utils::capitalise(expression->value);
      } else {
        output = expression->value;
      }
      break;
    }
    case Expression::Variant::FUNCTION_CALL: {
      std::string indent = Utils::get_indent(indentation);
      output += indent;
      if (is_built_in_fn(expression->value)) {
        output += get_built_in_fn(expression->value) + "(";
      } else {
        output += expression->value + "(";
      }

      for (size_t i = 0; i < expression->arguments.size(); i++) {
        output += handle_expression(expression->arguments[i]);
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

void Transpiler::handle_statement(const std::unique_ptr<Statement> &statement, const size_t &indentation) {
  if (statement->kind == Statement::Kind::EXPRESSION) {
    output += handle_expression(statement, indentation) + "\n";
    return;
  }

  std::string indent = Utils::get_indent(indentation); 
  switch (statement->type) {
    case Statement::Type::VARIABLE_DECLARATION: {
      auto variable = static_cast<Variable *>(statement.get());
      output += indent + variable->name + " = " + handle_expression(variable->value);
      break;
    }
    case Statement::Type::FUNCTION_DECLARATION: {
      auto function = static_cast<Function *>(statement.get());
      output += indent + "def " + function->name + "(";

      for (size_t i = 0; i < function->parameters.size(); i++) {
        output += function->parameters[i]->name;
        if (i < function->parameters.size() - 1) {
          output += ", ";
        }
      }

      output += "):\n";
      for (const auto &statement : function->children) {
        handle_statement(statement, indentation + 2);
      }
      break;
    }
    case Statement::Type::IF_STATEMENT: {
      auto if_statement = static_cast<If *>(statement.get());
      output += indent + "if " + handle_expression(if_statement->condition) + ":\n";
      for (const auto &statement : if_statement->children) {
        handle_statement(statement, indentation + 2);
      }

      if (if_statement->else_block) {
        output += indent + "else:\n";
        for (const auto &statement : if_statement->else_block->children) {
          handle_statement(statement, indentation + 2);
        }
      }

      break;
    }
    case Statement::Type::ELSE_STATEMENT: {
      throw std::runtime_error("DEV: Unpaired Else Statement");
      break;
    }
    case Statement::Type::LOOP_STATEMENT: {
      handle_loop_statement(statement, indentation);
      break;
    }
    default:      
      println("Statement Unsupported");
  }

  output += "\n";
}

// TODO: Implement Checker for better loop understaning and compiling
void Transpiler::handle_loop_statement(const std::unique_ptr<Statement> &statement, const size_t indentation) {
  if (statement->type != Statement::Type::LOOP_STATEMENT) {
    throw std::runtime_error("DEV: Expected Loop Statement");
  }

  auto loop = static_cast<For *>(statement.get());
  std::string indent = Utils::get_indent(indentation);

  if (loop->index && loop->index->literal == Token::Literal::FLOAT) {
    // TODO: Catch this in the Checker
    throw std::runtime_error("USER: Cannot use a float in a range loop");
  }

  if (loop->limit && loop->limit->literal == Token::Literal::FLOAT) {
    // TODO: Catch this in the Checker
    throw std::runtime_error("USER: Cannot use a float in a range loop");
  }

  switch (loop->variant) {
    case For::Variant::INFINITE: {
      output += indent + "while True:\n";
      break;
    }
    case For::Variant::TIMES: {
      std::unique_ptr<Expression> index = std::move(loop->index);
      output += indent + "for _ in range(" + handle_expression(index) + "):\n";
      break;
    }
    default: {
      std::unique_ptr<Expression> index = std::move(loop->index);
      std::unique_ptr<Expression> limit = std::move(loop->limit);

      if (limit->literal == Token::Literal::INTEGER) {
        output += indent + "for " + handle_expression(index) + " in range(" + handle_expression(limit) + "):\n";
      } else {
        output += indent + "for " + handle_expression(index) + " in " + handle_expression(limit) + ":\n";
      }
      
      break;    
    }
  }

  for (const auto &statement : loop->children) {
    handle_statement(statement, indentation + 2);
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
        output += handle_expression(statement) + "\n";
        break;
      }
      default:
        println("Kind Unsupported");
    }
  }

  Utils::write_file(output_path, output);
}