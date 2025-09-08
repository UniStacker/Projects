#include <iostream>
#include <cassert>
#include "core/lexer.h"


void test_keywords() {
  std::cout << "  * Testing keywords ...";
  std::string code = "if else while for continue break fn return";
  auto tokens = tokenize(code);

  assert(tokens.size() == 9); // 8 keywords + 1 EOF
  assert(tokens[0].type == "KEYWORD" && tokens[0].value == "if");
  assert(tokens[1].type == "KEYWORD" && tokens[1].value == "else");
  assert(tokens[2].type == "KEYWORD" && tokens[2].value == "while");
  assert(tokens[3].type == "KEYWORD" && tokens[3].value == "for");
  assert(tokens[4].type == "KEYWORD" && tokens[4].value == "continue");
  assert(tokens[5].type == "KEYWORD" && tokens[5].value == "break");
  assert(tokens[6].type == "KEYWORD" && tokens[6].value == "fn");
  assert(tokens[7].type == "KEYWORD" && tokens[7].value == "return");
  assert(tokens[8].type == "EOF");
}

void test_numbers_and_ops() {
  std::cout << "  * Testing numbers & operators ...";
  std::string code = "42 + 3.14";
  auto tokens = tokenize(code);

  assert(tokens.size() == 4); // 3 tokens + 1 EOF
  assert(tokens[0].type == "NUMBER" && tokens[0].value == "42");
  assert(tokens[1].type == "OPERATOR" && tokens[1].value == "+");
  assert(tokens[2].type == "NUMBER" && tokens[2].value == "3.14");
  assert(tokens[3].type == "EOF");
}

void test_identifiers() {
  std::cout << "  * Testing identifiers ...";
  std::string code = "foo bar123 _baz";
  auto tokens = tokenize(code);

  assert(tokens.size() == 4); // 3 identifiers + 1 EOF
  assert(tokens[0].type == "IDENT" && tokens[0].value == "foo");
  assert(tokens[1].type == "IDENT" && tokens[1].value == "bar123");
  assert(tokens[2].type == "IDENT" && tokens[2].value == "_baz");
  assert(tokens[3].type == "EOF");
}

void test_strings() {
  std::cout << "  * Testing strings ...";
  std::string code = R"("hello" 'world')";
  auto tokens = tokenize(code);

  assert(tokens.size() == 3); // 2 strings + 1 EOF
  assert(tokens[0].type == "STRING" && tokens[0].value == "hello");
  assert(tokens[1].type == "STRING" && tokens[1].value == "world");
  assert(tokens[2].type == "EOF");
}

void test_comments() {
  std::cout << "  * Testing comments ...";
  std::string code = "foo # this is a comment\nbar";
  auto tokens = tokenize(code);

  assert(tokens.size() == 4); // foo, NEWLINE, bar, EOF
  assert(tokens[0].type == "IDENT" && tokens[0].value == "foo");
  assert(tokens[1].type == "NEWLINE");
  assert(tokens[2].type == "IDENT" && tokens[2].value == "bar");
  assert(tokens[3].type == "EOF");
}

void test_punctuation() {
  std::cout << "  * Testing punctuation ...";
  std::string code = "(){},;:[]";
  auto tokens = tokenize(code);

  assert(tokens.size() == 10); // 9 punctuation + 1 EOF
  assert(tokens[0].type == "PUNCT" && tokens[0].value == "(");
  assert(tokens[1].type == "PUNCT" && tokens[1].value == ")");
  assert(tokens[2].type == "PUNCT" && tokens[2].value == "{");
  assert(tokens[3].type == "PUNCT" && tokens[3].value == "}");
  assert(tokens[4].type == "PUNCT" && tokens[4].value == ",");
  assert(tokens[5].type == "PUNCT" && tokens[5].value == ";");
  assert(tokens[6].type == "PUNCT" && tokens[6].value == ":");
  assert(tokens[7].type == "PUNCT" && tokens[7].value == "[");
  assert(tokens[8].type == "PUNCT" && tokens[8].value == "]");
  assert(tokens[9].type == "EOF");
}

void test_line_and_col_numbers() {
  std::cout << "  * Testing line & column numbers ...";
  std::string code = "foo\n  bar";
  auto tokens = tokenize(code);

  assert(tokens.size() == 4); // foo, NEWLINE, bar, EOF
  assert(tokens[0].type == "IDENT" && tokens[0].value == "foo" && tokens[0].line == 1 && tokens[0].col == 0);
  assert(tokens[1].type == "NEWLINE" && tokens[1].line == 1 && tokens[1].col == 3);
  assert(tokens[2].type == "IDENT" && tokens[2].value == "bar" && tokens[2].line == 2 && tokens[2].col == 2);
  assert(tokens[3].type == "EOF" && tokens[3].line == 2 && tokens[3].col == 5);
}

void test_unknown_chars() {
  std::cout << "  * Testing unknown characters ...";
  std::string code = "@ $&";
  auto tokens = tokenize(code);

  assert(tokens.size() == 4); // 3 UNKNOWN + 1 EOF
  assert(tokens[0].type == "UNKNOWN" && tokens[0].value == "@");
  assert(tokens[1].type == "UNKNOWN" && tokens[1].value == "$");
  assert(tokens[2].type == "UNKNOWN" && tokens[2].value == "&");
  assert(tokens[3].type == "EOF");
}

void test_all_operators() {
  std::cout << "  * Testing all operators ...";
  std::string code = "== != < > <= >= + - * ^ / // % = | and or not in";
  auto tokens = tokenize(code);

  assert(tokens.size() == 20); // 19 operators + 1 EOF
  assert(tokens[0].type == "OPERATOR" && tokens[0].value == "==");
  assert(tokens[1].type == "OPERATOR" && tokens[1].value == "!=");
  assert(tokens[2].type == "OPERATOR" && tokens[2].value == "<");
  assert(tokens[3].type == "OPERATOR" && tokens[3].value == ">");
  assert(tokens[4].type == "OPERATOR" && tokens[4].value == "<=");
  assert(tokens[5].type == "OPERATOR" && tokens[5].value == ">=");
  assert(tokens[6].type == "OPERATOR" && tokens[6].value == "+");
  assert(tokens[7].type == "OPERATOR" && tokens[7].value == "-");
  assert(tokens[8].type == "OPERATOR" && tokens[8].value == "*");
  assert(tokens[9].type == "OPERATOR" && tokens[9].value == "^");
  assert(tokens[10].type == "OPERATOR" && tokens[10].value == "/");
  assert(tokens[11].type == "OPERATOR" && tokens[11].value == "//");
  assert(tokens[12].type == "OPERATOR" && tokens[12].value == "%");
  assert(tokens[13].type == "OPERATOR" && tokens[13].value == "=");
  assert(tokens[14].type == "OPERATOR" && tokens[14].value == "|");
  assert(tokens[15].type == "OPERATOR" && tokens[15].value == "and");
  assert(tokens[16].type == "OPERATOR" && tokens[16].value == "or");
  assert(tokens[17].type == "OPERATOR" && tokens[17].value == "not");
  assert(tokens[18].type == "OPERATOR" && tokens[18].value == "in");
  assert(tokens[19].type == "EOF");
}

void test_string_with_escaped_quotes() {
  std::cout << "  * Testing string with escaped quotes ...";
  std::string code = R"("hello \"world\"")";
  auto tokens = tokenize(code);

  assert(tokens.size() == 2); // 1 string + 1 EOF
  assert(tokens[0].type == "STRING" && tokens[0].value == "hello \\\"world\\\"");
  assert(tokens[1].type == "EOF");
}

void run_lexer_tests() {
  std::cout << "Running lexer tests:\n";
  test_keywords();
  std::cout << " Done.\n";
  test_numbers_and_ops();
  std::cout << " Done.\n";
  test_identifiers();
  std::cout << " Done.\n";
  test_strings();
  std::cout << " Done.\n";
  test_comments();
  std::cout << " Done.\n";
  test_punctuation();
  std::cout << " Done.\n";
  test_line_and_col_numbers();
  std::cout << " Done.\n";
  test_unknown_chars();
  std::cout << " Done.\n";
  test_all_operators();
  std::cout << " Done.\n";
  test_string_with_escaped_quotes();
  std::cout << " Done.\n";
  
  std::cout << "✅ All lexer tests passed!\n";
}
