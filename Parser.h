#pragma once

#include <memory>
#include "Lexer.cpp"
#include "Statement.cpp"

class Parser {
  public:
    static PeekVectorPtr<Statement> build_block(
      Stream &stream, 
      const size_t &start_index, 
      bool is_main_program = false
    );
    
    static Statement parse(const std::string &file_path);
};