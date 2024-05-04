#pragma once

#include "Utils.h"
#include "Statement.h"
#include "Variable.h"

class Function : public Statement {
  public:
    std::string name;
    std::vector<std::unique_ptr<Variable>> parameters;

    static PeekPtr<Function> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};