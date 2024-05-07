#pragma once

#include "For.h"
#include "Parser.h"

PeekPtr<For> For::build(Stream &stream, const size_t &start_index) {
  PeekPtr<For> result;

  Token keyword = stream[start_index];
  if (not keyword.is_given_keyword(Keyword::FOR)) {
    throw std::runtime_error("DEV: Expected 'for' keyword");
  }

  Token next = stream.get_next(start_index);

  // for {}
  if (next.is_given_marker(Marker::LEFT_BRACE)) {
    PeekVectorPtr<Statement> body = Parser::build_block(stream, start_index + 1);

    result.data->variant = For::Variant::INFINITE;
    result.data->children = std::move(body.data);
    result.end_index = body.end_index;
    return result;
  }

  if (Expression::is_expression(stream, start_index)) {
    PeekPtr<Expression> index = Expression::build(stream, start_index);
    result.data->index = std::move(index.data);
    result.end_index = index.end_index;
  } 
  
  if (stream.is_next(result.end_index, Keyword::IN)) {
    PeekPtr<Expression> limit = Expression::build(stream, result.end_index + 1);

    // for <index> in <limit> {}
    result.data->limit = std::move(limit.data);
    result.data->variant = For::Variant::FOR_IN;
    result.end_index = limit.end_index;
  } else {
    // for <index> {}
    result.data->variant = For::Variant::TIMES;
  }

  PeekVectorPtr<Statement> body = Parser::build_block(stream, result.end_index + 1);
  result.data->children = std::move(body.data);
  result.end_index = body.end_index;

  return result;
}

void For::print(size_t indent) const {
  std::string indentation = Utils::get_indent(indent);
  println(indentation + "For {");

  if (variant == Variant::INFINITE) {
    println(indentation + "  variant: Infinite");
  } else if (variant == Variant::TIMES) {
    println(indentation + "  variant: Times");
  } else {
    println(indentation + "  variant: For");
  }

  if (index) {
    println(indentation + "  Index {");
    index->print(indent + 2);
    println(indentation + "  }");
  }

  if (limit) {
    println(indentation + "  Limit {");
    limit->print(indent + 2);
    println(indentation + "  }");
  }

  if (not children.empty()) {
    println(indentation + "  children: [");
    for (const auto &child : children) child->print(indent + 2);
    println(indentation + "  ]");
  }

  println(indentation + "}");
}