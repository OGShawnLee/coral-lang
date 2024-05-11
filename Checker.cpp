#pragma once

#include "Utils.h"
#include "Checker.h"

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

std::shared_ptr<Scope> Scope::create(std::shared_ptr<Scope> &parent) {
  auto scope = std::make_shared<Scope>();
  scope->failed = false;
  scope->parent = parent;
  return scope;
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

void Checker::check_expression(
  const std::unique_ptr<Statement> &element,
  std::shared_ptr<Scope> &current_scope
) {
  const auto expression = static_cast<Expression*>(element.get());
  switch (expression->variant) {
    case Expression::Variant::IDENTIFIER: {
      if (current_scope->is_undefined(expression->value)) {
        println("Undefined Identifier '" + expression->value + "'");
        global_scope->failed = failed = true;
      }
    } break;
    case Expression::Variant::FUNCTION_CALL: {
      if (is_built_in_fn(expression->value)) {
        break;
      }

      if (current_scope->is_undefined(expression->value)) {
        println("Undefined Function '" + expression->value + "'");
        global_scope->failed = failed = true;
      }
    } break;
    default:
      println("Unhandled Expression Variant");
  }
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
        variable->typing, 
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
  }
}

Checker::Checker(const Statement &element) {
  failed = false;
  global_scope = std::make_shared<Scope>();
  
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