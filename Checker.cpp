#pragma once

#include "Utils.h"
#include "Checker.h"

std::map<std::string, std::string> BUILT_IN_FN = {
  {"println", "print"},
  {"readln", "input"},
  {"str", "str"},
  {"int", "int"},
  {"float", "float"},
  {"bool", "bool"},
  {"len", "len"},
};

std::string get_built_in_fn(const std::string &name) {
  return BUILT_IN_FN.at(name);
}

bool is_built_in_fn(const std::string &name) {
  return BUILT_IN_FN.find(name) != BUILT_IN_FN.end();
}

std::shared_ptr<Scope> Scope::create(std::shared_ptr<Scope> &parent) {
  auto scope = std::make_shared<Scope>();
  scope->failed = false;
  scope->parent = parent;
  return scope;
}

Typing Scope::get_typing(std::string name) {
  if (Utils::has_key(entities, name)) {
    return entities.at(name);
  }

  if (parent == nullptr) {
    return Typing::create(Token::Literal::UNKNOWN);
  }

  return parent->get_typing(name);
}

bool Scope::is_duplicate(std::string name) {
  return Utils::has_key(entities, name);
}

bool Scope::is_undefined(std::string name) {
  if (is_duplicate(name)) {
    return false;
  }

  if (parent == nullptr) {
    return true;
  }

  return parent->is_undefined(name);
}

void Scope::append(std::string name, const Typing &type, Entity entity) {
  switch (entity) {
    case Entity::CONSTANT:
    case Entity::VARIABLE: {
      std::string entity_name = entity == Entity::CONSTANT ? "Constant" : "Variable";

      if (is_duplicate(name)) {
        println(entity_name + " '" + name + "' has been already declared.");
        failed = true;
      }

      entities[name] = type;
    } break;
    case Entity::FUNCTION: {
      if (is_duplicate(name)) {
        println("Function '" + name + "' has been already declared.");
        failed = true;
      }

      entities[name] = type;
    }
  }
}

Typing Checker::check_binary_expression(
  const BinaryExpression *element,
  std::shared_ptr<Scope> &current_scope
) {
  switch (element->variant) {
    case Expression::Variant::ASSIGNMENT: {
      Typing left = check_expression(element->left, current_scope);
      Typing right = check_expression(element->right, current_scope);

      if (right.data != left.data) {
        println("Unable to Assign: " + element->left->value + " to " + element->right->value);
        println("\tType Mismatch: " + left.value + " and " + right.value);
        global_scope->failed = failed = true;
        return Typing::create(Token::Literal::UNKNOWN);
      }

      return left;
    } break;
  }

  return Typing::create(Token::Literal::UNKNOWN);
}

Typing Checker::check_expression(
  const std::unique_ptr<Statement> &element,
  std::shared_ptr<Scope> &current_scope
) {
  const auto expression = static_cast<Expression*>(element.get());
  return check_expression(expression, current_scope);
}

Typing Checker::check_expression(
  const std::unique_ptr<Expression> &element,
  std::shared_ptr<Scope> &current_scope
) {
  return check_expression(element.get(), current_scope);
}

Typing Checker::check_expression(
  const Expression *element,
  std::shared_ptr<Scope> &current_scope
) {
  switch (element->variant) {
    case Expression::Variant::IDENTIFIER: {
      if (current_scope->is_undefined(element->value)) {
        println("Undefined Identifier '" + element->value + "'");
        global_scope->failed = failed = true;
        return Typing::create(Token::Literal::UNKNOWN);
      }

      return current_scope->get_typing(element->value);
    } break;
    case Expression::Variant::FUNCTION_CALL: {
      if (is_built_in_fn(element->value)) {
        return global_scope->get_typing(get_built_in_fn(element->value));
      }

      if (current_scope->is_undefined(element->value)) {
        println("Undefined Function '" + element->value + "'");
        global_scope->failed = failed = true;
        return Typing::create(Token::Literal::UNKNOWN);
      }
    } break;
    case Expression::Variant::ASSIGNMENT: {
      return check_binary_expression(
        static_cast<const BinaryExpression*>(element), 
        current_scope
      );
    } break;
    case Expression::Variant::LITERAL: 
      return Typing::create(element->literal);
      break;
    default:
      println("Unhandled Expression Variant");
  }

  return Typing::create(Token::Literal::UNKNOWN);
}

void Checker::check_statement(
  const std::unique_ptr<Statement> &element,
  std::shared_ptr<Scope> &current_scope
) {
  if (element->kind == Statement::Kind::EXPRESSION) {
    check_expression(element, current_scope);
    return;
  }

  switch (element->type) {
    case Statement::Type::CONSTANT_DECLARATION: 
    case Statement::Type::VARIABLE_DECLARATION: {
      const auto variable = static_cast<Variable*>(element.get());

      current_scope->append(
        variable->name, 
        check_expression(variable->value, current_scope),
        variable->is_constant ? Scope::Entity::CONSTANT : Scope::Entity::VARIABLE
      );
    } break;
    case Statement::Type::FUNCTION_DECLARATION: {
      const auto function = static_cast<Function*>(element.get());
      const Typing typing = function->typing;
      current_scope->append(function->name, typing, Scope::Entity::FUNCTION);

      auto child_scope = Scope::create(current_scope);
      child_scope->append(function->name, typing, Scope::Entity::FUNCTION);

      for (const auto &parameter : function->parameters) {
        const Typing typing = parameter->typing;
        child_scope->append(parameter->name, typing, Scope::Entity::CONSTANT);
      }

      for (const auto &child : function->children) {
        check_statement(child, child_scope);
      }

      if (child_scope->failed) {
        global_scope->failed = failed = true;
      }
    } break;
    case Statement::Type::IF_STATEMENT: {
      const auto if_statement = static_cast<If*>(element.get());
      check_expression(if_statement->condition, current_scope);

      auto child_scope = Scope::create(current_scope);

      for (const auto &child : if_statement->children) {
        check_statement(child, child_scope);
      }

      if (child_scope->failed) {
        global_scope->failed = failed = true;
      }
    } break;
  }
}

Checker::Checker(const Statement &element) {
  failed = false;
  global_scope = std::make_shared<Scope>();

  global_scope->append("print", Typing::create(Token::Literal::VOID), Scope::Entity::FUNCTION);
  global_scope->append("input", Typing::create(Token::Literal::STRING), Scope::Entity::FUNCTION);
  global_scope->append("str", Typing::create(Token::Literal::STRING), Scope::Entity::FUNCTION);
  global_scope->append("int", Typing::create(Token::Literal::INTEGER), Scope::Entity::FUNCTION);
  global_scope->append("float", Typing::create(Token::Literal::FLOAT), Scope::Entity::FUNCTION);
  global_scope->append("bool", Typing::create(Token::Literal::BOOLEAN), Scope::Entity::FUNCTION);
  global_scope->append("len", Typing::create(Token::Literal::INTEGER), Scope::Entity::FUNCTION);

  for (const auto &child : element.children) {
    if (child->kind == Statement::Kind::STATEMENT) {
      check_statement(child, global_scope);
    }

    if (child->kind == Statement::Kind::EXPRESSION) {
      check_expression(child, global_scope);
    }
  }

  if (global_scope->failed) {
    failed = true;
  }

  if (failed) {
    throw std::runtime_error("USER: Unable to Transpile Invalid Source");
  }
}