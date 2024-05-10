#pragma once

#include "Utils.h"
#include "Statement.h"
#include "Variable.h"

class For : public Statement {
  public:
    enum class Variant {
      // for <int> {}
      TIMES,
      // for <int> in <int> {}
      FOR_IN,
      INFINITE,
      // for {}
    };

    std::unique_ptr<Expression> index;
    std::unique_ptr<Expression> limit;
    Variant variant;

    static PeekPtr<For> build(Stream &stream, const size_t &start_index);

    virtual void print(size_t indent = 0) const;
};