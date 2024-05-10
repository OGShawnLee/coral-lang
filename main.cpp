// #include "Parser.cpp"
#include "Transpiler.cpp"

int main() {
  // Statement program = Parser::parse("main.pino");
  // program.print();
  
  Transpiler handler;
  handler.transpile("main.pino", "main.py");

  return 0;
}