#pragma once

#include "Utils.h"
#include "Checker.h"

Scope::Scope() {
  failed = false;
}

bool Scope::is_duplicate(std::string name) {
  return Utils::has_key(entities, name);
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

void Checker::check_statement(
  const std::unique_ptr<Statement> &element,
  Scope &scope
) {
  switch (element->type) {
    case Statement::Type::CONSTANT_DECLARATION: 
    case Statement::Type::VARIABLE_DECLARATION: {
      const auto variable = static_cast<Variable*>(element.get());
      
      scope.append(
        variable->name, 
        variable->typing, 
        variable->is_constant ? Scope::Entity::CONSTANT : Scope::Entity::VARIABLE
      );
    } break;
    case Statement::Type::FUNCTION_DECLARATION: {
      const auto function = static_cast<Function*>(element.get());
      const Typing typing = function->typing;
      global_scope.append(function->name, typing, Scope::Entity::FUNCTION);

      Scope child_scope;
      child_scope.append(function->name, typing, Scope::Entity::FUNCTION);

      for (const auto &parameter : function->parameters) {
        const Typing typing = parameter->typing;
        child_scope.append(parameter->name, typing, Scope::Entity::CONSTANT);
      }

      for (const auto &child : function->children) {
        check_statement(child, child_scope);
      }

      if (child_scope.failed) {
        global_scope.failed = failed = true;
      }
    } break;
  }
}

Checker::Checker(const Statement &element) {
  failed = false;

  for (const auto &child : element.children) {
    if (child->kind == Statement::Kind::STATEMENT) {
      check_statement(child, global_scope);
    }

    if (child->kind == Statement::Kind::EXPRESSION) {
      // TODO: Check Expression
    }
  }

  if (global_scope.failed) {
    failed = true;
  }

  if (failed) {
    throw std::runtime_error("USER: Unable to Transpile Invalid Source");
  }
}