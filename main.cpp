// #include "Parser.cpp"
// #include "Checker.cpp"
#include "Transpiler.cpp"

int main() {
  // Statement program = Parser::parse("main.pino");
  // Checker checker(program);
  // program.print();
  
  Transpiler handler;
  handler.transpile("main.pino", "main.py");

  return 0;
}