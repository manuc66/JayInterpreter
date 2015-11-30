// StaticTypeCheck.h
// Static type checking for Jay is defined by the functions 
// V and the auxiliary functions typing and typeOf.  These
// functions use the classes in the Abstract Syntax of Jay.
#ifndef StaticTypeCheck_H
#define StaticTypeCheck_H

#include <map>

#include "TokenStream.hh"
#include "ConcreteSyntax.hh"
#include "AbstractSyntax.hh"


class TypeMap {
  typedef std::map<const std::string,Type> mtype;	// map type

public: 
  mtype m;  // map
  
  TypeMap() {}
  void display();

 private:
  // Prevent forbidden actions:
  TypeMap(const TypeMap&);
  TypeMap& operator=(const TypeMap&);  

};


class StaticTypeCheck {
public:
  StaticTypeCheck() {}
  bool V(const Program *p, TypeMap* tm);
  TypeMap* typing(const Declarations *d) const;

private:
  // TypeMap can't be const because map indexing is non-const:
  Type typeOf(const Expression *e, TypeMap *tm) const; 
  bool V(const Declarations *d) const; 
  bool V(Expression *e, TypeMap *tm);
  bool V(Statement *s, TypeMap *tm);

  // Prevent forbidden actions:
  StaticTypeCheck(const StaticTypeCheck&);
  StaticTypeCheck& operator=(const StaticTypeCheck&);  
}; // class StaticTypeCheck

#endif

