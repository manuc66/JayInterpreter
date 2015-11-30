#ifndef ConcreteSyntax_H
#define ConcreteSyntax_H

#include <string>
#include "TokenStream.hh"
#include "AbstractSyntax.hh"


class ConcreteSyntax {
  // Recursive descent parser that inputs a Jay program and 
  // generates its abstract syntax.  Each method corresponds to
  // a concrete syntax grammar rule, which appears as a comment
  // at the beginning of the method.
  
public: 
  ConcreteSyntax(TokenStream &ts) : input(ts), token(input.nextToken()) {}
  // Open the Jay source program as a TokenStream, and retrieve its first Token

  ~ConcreteSyntax() {} 

  Program* program();

private: 
  void match(const std::string& s);
  
  Declarations* declarations();
  void declaration(Declarations* ds);
  
  Type type();
  
  void identifiers(Declarations* ds, Type t);
  
  Statement* statement();  
  Block* statements();
  
  Assignment* assignment();
  Expression* expression();
  Expression* conjunction();
  Expression* relation(); 
  Expression* addition(); 
  Expression* term();
  Expression* date(Expression* day);
  Expression* negation();

  Expression* factor();
  
  Conditional* ifStatement();  
  Loop* whileStatement();
  
  bool isInteger(std::string s) const;

  void syntaxError(std::string tok);

  TokenStream& input;
  Token token;          // current token from the input stream

  // Prevent forbidden actions:
  ConcreteSyntax(const ConcreteSyntax&);
  ConcreteSyntax& operator=(const ConcreteSyntax&);
};

#endif

