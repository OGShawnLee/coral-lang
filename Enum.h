#pragma once

#include "Utils.h"
#include "Statement.h"

class Enum : public Statement {
  public:
    std::string name;
    std::vector<std::string> values;

    static PeekPtr<Enum> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};