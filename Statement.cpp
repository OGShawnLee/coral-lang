#pragma once

#include <map>
#include "Statement.h"

std::map<Statement::Kind, std::string> KIND_NAME = {
  {Statement::Kind::PROGRAM, "Program"},
  {Statement::Kind::STATEMENT, "Statement"},
}; 

void Statement::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  std::string name = KIND_NAME.at(kind);

  println(indentation + name + " {");

  if (children.size() > 0) {
    println(indentation + "  children: [");
    for (const auto &child : children) {
      child->print(indent + 2);
    }
    println(indentation + "  ]");
  }

  println(indentation + "}");
}