// Semantics
#ifndef Semantics_H
#define Semantics_H

#include <map>
#include <iostream>
#include "AbstractSyntax.hh"


class State {
  // Defines the set of variables and their associated values
  // that are active during interpretation: State is implemented as a STL map

  typedef std::map<std::string,Value> mtype2; // map type
 public:
  mtype2 ms;


  // construct the empty state
  State() {}
  ~State();

  // construct a new state with a single key-value pair
  State(Variable *key, Value val) {
    ms[key->id] = val;       /* or  ms.insert(mtype2::value_type(key->id, val));*/
  }

  // Overriding union function "onion" on two states s and t;
  // returns a state composed of all elements in t together
  // with all elements in s that are not in t.    
  State* onion(const State& t);
  void display();

 private:
  // Prevent forbidden actions:
  State (const State&);
  State& operator=(const State&);
};

// Following is the semantics class, characterized by the meaning
// function M and its auxiliary function applyBinary.  

class Semantics {
 public:
  Semantics () {}
  State* M(Program *p);
  State* initialState(const Declarations *d) const;
  
  Value applyBinary(const Operator *op, const Value &v1, const Value &v2) const;
  Value applyUnary(const Operator *op, const Value &v) const;
  
  Value M(Expression *e, State *sigma);
  State* M(Statement *s, State *sigma);
  State* M(Skip *s, State *sigma); 
  State* M(Assignment *a, State *sigma);
  State* M(Block *b, State *sigma);
  State* M(Conditional *c, State *sigma); 
  State* M(Loop *l, State *sigma);

 private:
  std::string itoa(const int x) const;
  // Prevent forbidden actions:
  Semantics (const Semantics&);
  Semantics& operator=(const Semantics&);
};

#endif
