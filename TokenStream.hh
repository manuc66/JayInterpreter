#ifndef TokenStream_H
#define TokenStream_H

#include <iostream>
#include <fstream>
#include <string>


class Token {
public:
  Token(std::string t = "Other", std::string v = "") : type(t), value(v) {}
  Token(std::string t, char v) : type(t), value(std::string(1, v)) {}

  std::string type;	  /* token type: Identifier, Keyword, Literal,
			     Separator, Operator, or Other */
  std::string value;		// token value
};


class TokenStream {
public:
  // Pass a filename for the program text as a source for the TokenStream:
  TokenStream(const char *fileName);

  Token nextToken();
  bool eof() const;

private: 
  char readChar();
  bool isKeyword(const std::string& s) const;
  bool isWhiteSpace(char c) const;
  bool isEndOfLine(char c) const; 
  void skipWhiteSpace(); 
  bool isSeparator(char c) const;
  bool isOperator(char c) const;
  bool isLetter(char c) const; 
  bool isDigit(char c) const;

  bool isEof;
  char nextChar;		// next character in input stream
  std::ifstream input;		// file

  // Prevent forbidden actions:
  TokenStream(const TokenStream&);
  TokenStream& operator=(const TokenStream&);
};

#endif
