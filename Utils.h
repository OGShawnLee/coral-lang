#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

template <typename T>
struct Peek {
  T data;
  size_t end_index;
};

template <typename T>
struct PeekPtr {
  std::unique_ptr<T> data;
  size_t end_index;

  PeekPtr() {
    data = std::unique_ptr<T>(new T());
  }
};

template <typename T>
struct PeekVectorPtr {
  std::vector<std::unique_ptr<T>> data;
  size_t end_index;

  PeekVectorPtr() {
    data = std::vector<std::unique_ptr<T>>();
  }
};

void printsln(const std::string &line) {
  std::cout << line;
}

void println(const std::string &line) {
  std::cout << line << std::endl;
}

namespace Utils {
  template <typename T>
  bool included(const std::vector<T> &vector, const T &element) {
    return std::find(vector.begin(), vector.end(), element) != vector.end();
  }

  template <typename T>
  bool has_key(const std::map<std::string, T> &map, const std::string &key) {
    return map.find(key) != map.end();
  }

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

  std::string get_indent(size_t indent) {
    std::string result;
    for (size_t i = 0; i < indent; i++) result += "  ";
    return result;
  }

  std::string join(const std::vector<std::string> &strings, const std::string &separator) {
    std::string result;
    for (size_t i = 0; i < strings.size(); i++) {
      result += strings[i];
      if (i + 1 < strings.size()) result += separator;
    }
    return result;
  }
}