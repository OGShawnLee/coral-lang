#include "Lexer.cpp"

int main() {
  Stream stream = Lexer::lex_file("main.pino");
  for (const Token &token : stream) token.print();

  return 0;
}