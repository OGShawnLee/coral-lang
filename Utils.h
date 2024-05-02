#pragma once

#include <fstream>
#include <functional>
#include <iostream>

void println(const std::string &line) {
  std::cout << line << std::endl;
}

namespace Utils {
  bool is_whitespace(char character) {
    return character == ' ' || character == '\t' || character == '\n';
  }

  bool is_whitespace(const std::string &line) {
    for (char character : line) 
      if (not is_whitespace(character)) 
        return false;

    return true;
  }

  void each_line(const std::string &file_path, std::function<void(const std::string &)> fn) {
    std::ifstream file(file_path);
    std::string line;
    while (std::getline(file, line)) fn(line);
  }
}