#include <fstream>
#include <string>
#include <stdexcept>
#include "TokenStream.hh"

using namespace std;


TokenStream::TokenStream(const char *fileName)
  : isEof(false), nextChar(' '), input(fileName) {
  if (!input)
    throw std::runtime_error("File not found: " + string(fileName));
}


Token TokenStream::nextToken() { // Return next token type and value
  skipWhiteSpace();	   // first check for whitespace and bypass it

  // Then check for a comment, and bypass it
  // but remember that / is also a division operator
  while (nextChar == '/') {
    nextChar = readChar();
    if (nextChar == '/') {	// if / is followed by another /
      nextChar = readChar();	// skip rest of line -- it's a comment
      while (!isEndOfLine(nextChar))
	nextChar = readChar();	// look for <cr>, <lf>, <ff>
      skipWhiteSpace();
    }
    else      // a slash followed by anything else must be an operator
      return Token("Operator", "/");
  }

  // Then check for an Operator; recover 2-character operators
  // as well as 1-character ones
  if (isOperator(nextChar)) {
    Token t("Operator", nextChar);
    switch (nextChar) {
    case '<': case '>': case '!': case '=':
      // look for <=, >=, !=, ==
      nextChar = readChar();
      if (nextChar == '=') {
	t.value = t.value + nextChar;
	nextChar = readChar();
      }
      return t;
    case '&':                     // look for &&
      nextChar = readChar();
      if (nextChar == '&') {
	t.value = t.value + nextChar;
	nextChar = readChar();
      }
      return t;
    case '|':                    // look for ||
      nextChar = readChar();
      if (nextChar == '|') {
	t.value = t.value + nextChar;
	nextChar = readChar();
      }
      return t;
    default:                    // all other operators
      nextChar = readChar();
      return t;
    }
  }
 
  // Then check for a Separator
  if (isSeparator(nextChar)) {
    Token t("Separator", nextChar);
    nextChar = readChar();
    return t;
  }
 
  // Then check for an Identifier, Keyword, or Literal
  if (isLetter(nextChar)) { // get an Identifier
    Token t("Identifier");
    while ((isLetter(nextChar) || isDigit(nextChar))) {
      t.value = t.value + nextChar;
      nextChar = readChar();
    }
    // now see if this is a Keyword
    if (isKeyword(t.value))
      t.type = "Keyword";
    // check for a bool literal
    else if (t.value == "true" || t.value == "false")
      t.type = "Literal";
    return t;
  }
 
  if (isDigit(nextChar)) { // check for numbers
    Token t("Literal");
    while (isDigit(nextChar)) {
      t.value = t.value + nextChar;
      nextChar = readChar();
    }
    return t;
  }

  // In case of a lexical error:
  Token t;

  if (isEof) return t;

  t.value += nextChar;
  nextChar = readChar();
  return t;
}


char TokenStream::readChar() {
  if (isEof) return 0;

  char i;
  input.read(&i, 1);

  if (input.eof()) {
    isEof = true;
    return 0;
  }
  
  return i;
}


bool TokenStream::isKeyword(const string& s) const {
  return (s == "bool" || s == "else" || s == "if" || s == "int" ||
	  s == "main" || s == "void" || s == "while" || s == "date"  );
}


bool TokenStream::isWhiteSpace(char c) const {
  return (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f');
}


bool TokenStream::isEndOfLine(char c) const {
  return (c == '\r' || c == '\n' || c == '\f' || isEof);
}


void  TokenStream::skipWhiteSpace() {
  // check for whitespace, and bypass it
  while (!isEof && isWhiteSpace(nextChar))
    nextChar = readChar();
}


bool TokenStream::isSeparator(char c) const {
  return (c == '[' || c == ']' || c == '(' || c == ')' || c == ':' ||
	  c == '{' || c == '}' || c == ';' || c == ',' || c == '.'   );
}

bool  TokenStream::isOperator(char c) const {
  return (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' ||
	  c == '<' || c == '>' || c == '!' || c == '&' || c == '|'   );
}


bool TokenStream::isLetter(char c) const {
  return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');    
}


bool TokenStream::isDigit(char c) const {
  return (c >= '0' && c <= '9');
}


bool  TokenStream::eof() const {
  return isEof;
}
