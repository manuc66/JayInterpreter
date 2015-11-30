// StaticTypeCheck.cpp

#include "StaticTypeCheck.hh"

using namespace std;

// Static type checking for Jay is defined by the functions 
// V and the auxiliary functions typing and typeOf.  These
// functions use the classes in the Abstract Syntax of Jay.


void TypeMap::display() {
  cout << "{ ";
  for (mtype::iterator iter = m.begin(); iter != m.end(); ) {
    cout << "<" << iter->first << ", " << iter->second << ">";
    iter++;
    if (iter != m.end())
      cout << ", ";
  }
  cout << " }";
}


TypeMap* StaticTypeCheck::typing(const Declarations *d) const {
  // put the variables and their types into a new 
  // Dictionary (symbol table) and return it.

  TypeMap *map = new TypeMap();

  for (unsigned int i = 0; i < d->vec.size(); i++)
    map->m[d->vec[i]->v->id] = d->vec[i]->t;
  return map;
} 


bool StaticTypeCheck::V(const Declarations *d) const  {
  for (unsigned int i = 0; i < d->vec.size() - 1; i++)
    for (unsigned int j = i + 1; j < d->vec.size(); j++)
      if (d->vec[i]->v == d->vec[j]->v)
	return false;
  return true;
} 


bool StaticTypeCheck::V(const Program* p, TypeMap* tm) {
  return V(p->decpart) && V(p->body, tm);
} 


Type StaticTypeCheck::typeOf(const Expression* e, TypeMap* tm) const {
  if (e->name() == "Value") return dynamic_cast<const Value*>(e)->type;
  if (e->name() == "Variable") {
    if (tm->m.find(dynamic_cast<const Variable*>(e)->id) == tm->m.end())
      return Type(Type::UNDEFINED);
    else return tm->m[dynamic_cast<const Variable*>(e)->id];
  }
  if (e->name() == "Binary") {
    const Binary* b = dynamic_cast<const Binary*>(e);
    // Define the type of and ArithmeticOp
    if (b->op->ArithmeticOp()) {
      // type of ArithmeticOp beetween integer ONLY is Integer
      if (typeOf(b->term1, tm).isInteger() &&
	  typeOf(b->term2, tm).isInteger()   )
	return Type(Type::INTEGER);
      // type of ArithmeticOp beetween integer AND Date is Integer
      else if ((typeOf(b->term1, tm).isInteger() &&
		typeOf(b->term2, tm).isDate()      ) ||
	       (typeOf(b->term1, tm).isDate()    &&
		typeOf(b->term2, tm).isInteger()   ))
	return Type(Type::DATE);
      else if (typeOf(b->term1, tm).isDate() &&
	       typeOf(b->term2, tm).isDate() &&
	       (b->op->val == "-")                 )
	return Type(Type::INTEGER);
    }
    if (b->op->RelationalOp() || b->op->BoolOp())
      return Type(Type::BOOLEAN);
  }
  if (e->name() == "Unary") {
    if (dynamic_cast<const Unary*>(e)->op->UnaryOp()) 
      return Type(Type::BOOLEAN);
  }
  if (e->name() == "Date") {
    return Type(Type::DATE);
  }
  return Type(Type::UNDEFINED);
} 


bool StaticTypeCheck::V(Expression *e, TypeMap *tm) {
  if (e->name() == "Value") {
    Value *v = dynamic_cast<Value*>(e);
    if (v->type.isDate()) {
      // check that date contain 3 expression and each of them has an Integer value
      return V(v->day, tm) && 
	V(v->month, tm) &&
	V(v->year, tm) &&
	typeOf(v->day, tm).isInteger()   &&
	typeOf(v->month, tm).isInteger() &&
	typeOf(v->year, tm).isInteger();
    }
    else
      return true;
  }
  if (e->name() == "Variable") { // variable
    return (tm->m.find(dynamic_cast<Variable*>(e)->id) != tm->m.end());
  }
  if (e->name() == "Binary") {
    if (! V(dynamic_cast<Binary*>(e)->term1, tm)) return false;
    if (! V(dynamic_cast<Binary*>(e)->term2, tm)) return false;
    // define which arithmetic operation are allowed :
    if (dynamic_cast<Binary*>(e)->op->ArithmeticOp()) {
      // ArithmeticOp beetween Integer ONLY
      if (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isInteger() &&
	  typeOf(dynamic_cast<Binary*>(e)->term2, tm).isInteger()   )
	return true;
      // ArithmeticOp beetween Integer and Date
      if (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isDate()    &&
	  typeOf(dynamic_cast<Binary*>(e)->term2, tm).isInteger()   )
	return true;
      // ArithmeticOp beetween Date and Integer
      if (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isInteger() &&
	  typeOf(dynamic_cast<Binary*>(e)->term2, tm).isDate()      )
	return true;
      // ArithmeticOp beetween Date and Date for "-" is allowed
      if (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isDate()    &&
	  typeOf(dynamic_cast<Binary*>(e)->term2, tm).isDate()    &&
	  (dynamic_cast<Binary*>(e)->op->val == "-")                    )
	return true;
    }
    // define which relation operation are allowed :
    if (dynamic_cast<Binary*>(e)->op->RelationalOp()) {
      // RelationalOp beetween Integer ONLY
      if (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isInteger() &&
	  typeOf(dynamic_cast<Binary*>(e)->term2, tm).isInteger()   )
	return true;
      // RelationalOp beetween Date ONLY
      if (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isDate() &&
	  typeOf(dynamic_cast<Binary*>(e)->term2, tm).isDate()   )
	return true;
      // RelationalOp beetween two bool for "==" and "!="
      if (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isBool() &&
	  typeOf(dynamic_cast<Binary*>(e)->term2, tm).isBool() &&
	  ((dynamic_cast<Binary*>(e)->op->val == "==") ||
	   (dynamic_cast<Binary*>(e)->op->val == "!=")   )       ) 
	return true;
      return false;
    }
    if (dynamic_cast<Binary*>(e)->op->BoolOp())
      return (typeOf(dynamic_cast<Binary*>(e)->term1, tm).isBool() &&
	      typeOf(dynamic_cast<Binary*>(e)->term2, tm).isBool()   );
  }
  if (e->name() == "Unary")
    return (typeOf(dynamic_cast<Unary*>(e)->term, tm).isBool() &&
	    V(dynamic_cast<Unary*>(e)->term, tm)               &&
	    dynamic_cast<Unary*>(e)->op->val == "!"              );
  return false;
}


bool StaticTypeCheck::V(Statement *s, TypeMap *tm) {
  if (s->name() == "Skip" || s == NULL) return true;

  if (s->name() == "Assignment") {

    bool b1 = (tm->m.find(dynamic_cast<Assignment*>(s)->target->id) !=
	       tm->m.end());
    bool b2 = V(dynamic_cast<Assignment*>(s)->source, tm);

    if (b1 && b2) {
      if (tm->m[dynamic_cast<Assignment*>(s)->target->id] ==  
	  typeOf(dynamic_cast<Assignment*>(s)->source, tm))
	return true;
    }
    return false;
  }

  if (s->name() == "Conditional")
    return (V(dynamic_cast<Conditional*>(s)->test, tm)               &&
	    typeOf(dynamic_cast<Conditional*>(s)->test, tm).isBool() &&
	    V(dynamic_cast<Conditional*>(s)->thenbranch, tm)         &&
	    V(dynamic_cast<Conditional*>(s)->elsebranch, tm)           );
 
  if (s->name() == "Loop")
    return (V(dynamic_cast<Loop*>(s)->test, tm)               &&
	    typeOf(dynamic_cast<Loop*>(s)->test, tm).isBool() &&
	    V(dynamic_cast<Loop*>(s)->body, tm)                 );

  if ((s->name()) == "Block") {
    for (unsigned int j=0; j < dynamic_cast<Block*>(s)->members.size(); j++)
      if (! V(dynamic_cast<Block*>(s)->members[j], tm))
	return false; 
    return true;
  }
  return false;
}
// class StaticTypeCheck
