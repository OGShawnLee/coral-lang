#include "Parser.cpp"

int main() {
  Statement program = Parser::parse("main.pino");
  program.print();
  return 0;
}