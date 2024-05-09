#pragma once

#include "Utils.h"
#include "Function.h"
#include "Statement.h"

class Enum : public Statement {
  public:
    Type type = Type::ENUM_DECLARATION;
    std::string name;
    std::vector<std::string> values;
    std::vector<std::unique_ptr<Function>> methods;

    static PeekPtr<Enum> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};