// #include "Parser.cpp"
#include "Transpiler.cpp"

int main() {
  Statement program = Parser::parse("index.pino");
  program.print();
  
  // Transpiler handler;
  // handler.transpile("index.pino", "main.py");

  return 0;
}