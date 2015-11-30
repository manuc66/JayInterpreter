// Semantics.cpp
#include "Semantics.hh"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;


State::~State() { }


State* State::onion(const State &t) {
  for(mtype2::const_iterator iter = t.ms.begin(); iter != t.ms.end(); iter++){
    ms[iter->first] = iter->second;
  }
  return this;
}


void State::display() {
  cout << "{ ";
  for (mtype2::iterator iter = ms.begin(); iter != ms.end(); ) {
    cout << "<" + iter->first + ", ";
    iter->second.display(-1);
    iter++;
    if (iter != ms.end())
      cout << ">, ";
    else cout<<">";
  }

  cout << " }\n";
}


State* Semantics::M(Program *p) {
  return M(p->body, initialState(p->decpart));
}


State* Semantics::initialState(const Declarations *d) const {
  State *sigma = new State();
//  Value *undef = new Value();

  for (unsigned int i=0; i < d->vec.size(); i++)
    sigma->ms[dynamic_cast<Declaration *>(d->vec[i])->v->id] =  Value();

  return sigma;
}


Value Semantics::applyBinary(const Operator *op,
			     const Value &v1, const Value &v2) const {
  if (v1.type.isUndefined() || v2.type.isUndefined()) {
    return Value();
  }
  else if (op->ArithmeticOp()) {
    if (v1.type.isInteger() &&  v2.type.isInteger()) {
      if (op->val == Operator::PLUS)
	return Value(v1.intValue + v2.intValue);
      if (op->val == Operator::MINUS)
	return Value(v1.intValue - v2.intValue);
      if (op->val == Operator::TIMES)
	return Value(v1.intValue * v2.intValue);
      if (op->val == Operator::DIV)
	return Value(v1.intValue / v2.intValue);
    }
    else if (v1.type.isDate() || v2.type.isDate()) {
      if (v1.type.isInteger()) {
	const Value* d = dynamic_cast<const Value*>(v2.day);
	Value vv = Value(new Value(v1.intValue + d->intValue), v2.month, v2.year);
	vv.normalizeDate();
	return vv;
      }
      else if (v2.type.isInteger()) {
	const Value* v = dynamic_cast<const Value*>(v1.day);
	Value vv = Value(new Value(v2.intValue + v->intValue), v1.month, v1.year);
	vv.normalizeDate();
	return vv;
      }
      else if (op->val == Operator::MINUS) {
	// the emplemented algorith is not good, but it works
	// i had not time to do better one ...

	// date 1
	const Value* d1 = dynamic_cast<const Value*>(v1.day);
	const Value* m1 = dynamic_cast<const Value*>(v1.month);
	const Value* y1 = dynamic_cast<const Value*>(v1.year);
	
	// date 2
	const Value* d2 = dynamic_cast<const Value*>(v2.day);
	const Value* m2 = dynamic_cast<const Value*>(v2.month);
	const Value* y2 = dynamic_cast<const Value*>(v2.year);
	int s1;
	int s2;
	string date_time_command;

	if ((y2->intValue < 1970) || (y2->intValue > 2057) || (y1->intValue < 1970) || (y1->intValue > 2057)) {
	  throw runtime_error("Diff of date can't be done, only for date with year beetween  1970 and 2057");
	}

	date_time_command =  "date -d \"" + itoa(m1->intValue) + "/" + itoa(d1->intValue) + "/" + itoa(y1->intValue) + "\" +%s > /tmp/time";
	if (system(date_time_command.c_str()) == 0) {
	  ifstream tempFile("/tmp/time");
	  tempFile >> s1;
	  tempFile.close();
	  remove("/tmp/time");
	  s1 = s1 / 60 / 60 / 24;
	}
	else 
	  s1 = d1->intValue;

	date_time_command =  "date -d \"" + itoa(m2->intValue) + "/" + itoa(d2->intValue) + "/" + itoa(y2->intValue) + "\" +%s > /tmp/time";
	if (system(date_time_command.c_str()) == 0) {
	  ifstream tempFile("/tmp/time");
	  tempFile >> s2;
	  tempFile.close();
	  remove("/tmp/time");
	  s2 = s2 / 60 / 60 / 24;
	}
	else 
	  s2 = d2->intValue;

	return Value(s1 - s2); 
      }
    }
  }
  else if (op->RelationalOp()) {
    if (v1.type.isInteger() && v2.type.isInteger()) {
      if (op->val == Operator::LT) 
	return Value(v1.intValue < v2.intValue);
      if (op->val == Operator::LE) 
	return Value(v1.intValue <= v2.intValue);
      if (op->val == Operator::EQ) 
	return Value(v1.intValue == v2.intValue);
      if (op->val == Operator::NE) 
	return Value(v1.intValue != v2.intValue);
      if (op->val == Operator::GE) 
	return Value(v1.intValue >= v2.intValue);
      if (op->val == Operator::GT) 
	return Value(v1.intValue > v2.intValue);
    }
    else if (v1.type.isBool() && v2.type.isBool()) {
      if (op->val == Operator::EQ) 
	return Value(v1.boolValue == v2.boolValue);
      if (op->val == Operator::NE) 
	return Value(v1.boolValue != v2.boolValue);
    }
    else if (v1.type.isDate() && v2.type.isDate()) {
      // date 1
      const Value* d1 = dynamic_cast<const Value*>(v1.day);
      const Value* m1 = dynamic_cast<const Value*>(v1.month);
      const Value* y1 = dynamic_cast<const Value*>(v1.year);

      // date 2
      const Value* d2 = dynamic_cast<const Value*>(v2.day);
      const Value* m2 = dynamic_cast<const Value*>(v2.month);
      const Value* y2 = dynamic_cast<const Value*>(v2.year);
      
      if (op->val == Operator::LT) { 
	if (y1->intValue < y2->intValue)
	  return Value(true);
	else if (y1->intValue > y2->intValue)
	  return Value(false);
	else {
	  if (m1->intValue < m2->intValue)
	    return Value(true);
	  else if (m1->intValue > m2->intValue)
	    return Value(false);
	  else
	    return Value(d1->intValue < d2->intValue);
	}
      }
      if (op->val == Operator::LE) {
	if ((y1->intValue < y2->intValue) || (d1->intValue == d2->intValue && m1->intValue == m2->intValue && y1->intValue == y2->intValue))
	  return Value(true);
	else if (y1->intValue > y2->intValue)
	  return Value(false);
	else {
	  if (m1->intValue < m2->intValue)
	    return Value(true);
	  else if (m1->intValue > m2->intValue)
	    return Value(false);
	  else
	    return Value(d1->intValue < d2->intValue);
	}
      }
      if (op->val == Operator::EQ) {
	return Value((m1->intValue == m2->intValue) && (m1->intValue == m2->intValue) && (y1->intValue == y2->intValue));
      }
      if (op->val == Operator::NE) {
	return Value((d1->intValue != d2->intValue) && (m1->intValue != m2->intValue) && (y1->intValue != y2->intValue));
      }
      if (op->val == Operator::GE) {
	if ((y1->intValue > y2->intValue) || (d1->intValue == d2->intValue && m1->intValue == m2->intValue && y1->intValue == y2->intValue))
	  return Value(true);
	else if (y1->intValue < y2->intValue)
	  return Value(false);
	else {
	  if (m1->intValue > m2->intValue)
	    return Value(true);
	  else if (m1->intValue < m2->intValue)
	    return Value(false);
	  else
	    return Value(d1->intValue > d2->intValue);
	}
      }
      if (op->val == Operator::GT) {
	if (y1->intValue > y2->intValue)
	  return Value(true);
	else if (y1->intValue < y2->intValue)
	  return Value(false);
	else {
	  if (m1->intValue > m2->intValue)
	    return Value(true);
	  else if (m1->intValue < m2->intValue)
	    return Value(false);
	  else
	    return Value(d1->intValue > d2->intValue);
	}
      }
    }
  } 
  else if (op->BoolOp()) {
    if (op->val == Operator::AND) 
      return Value(v1.boolValue && v2.boolValue);
    if (op->val == Operator::OR) 
      return Value(v1.boolValue || v2.boolValue);
  }
  return Value(); // NULL
} 
    

Value Semantics::applyUnary(const Operator *op, const Value& v) const {

  if (v.type.isUndefined())
    return Value();
  else if (op->val == Operator::NOT)
    return Value(!v.boolValue);
  return Value(); // NULL
}


Value Semantics::M(Expression *e, State *sigma) {
  if (e->name() == "Value") {
    Value* v = dynamic_cast<Value*>(e);
    if (v->type.isDate()) {
      Value* vv = new Value(new Value(M(v->day, sigma)), new Value(M(v->month, sigma)), new Value(M(v->year, sigma)));
      vv->normalizeDate();
      return *vv;
    }
    else  {
      return *v;
    }
  }
  else if (e->name() == "Variable") {
    if ((sigma->ms.find(dynamic_cast<Variable*>(e)->id) == sigma->ms.end())) {
      return Value();
    }
    else {
      return sigma->ms[dynamic_cast<Variable*>(e)->id];
    }
  }
  else if (e->name() == "Binary") {
    return applyBinary(dynamic_cast<Binary*>(e)->op,
		       M(dynamic_cast<Binary*>(e)->term1, sigma),
		       M(dynamic_cast<Binary*>(e)->term2, sigma));
  }
  else if (e->name() == "Unary") {
    return applyUnary(dynamic_cast<Unary*>(e)->op,
		      M(dynamic_cast<Unary*>(e)->term, sigma));
  }
  else {
    return Value(); //NULL
  }
}


State* Semantics::M(Statement *s, State *sigma) {
  if (s->name() ==  "Skip"       )  return M((Skip*       )s, sigma);
  if (s->name() ==  "Assignment" )  return M((Assignment* )s, sigma);
  if (s->name() ==  "Conditional")  return M((Conditional*)s, sigma);
  if (s->name() ==  "Loop"       )  return M((Loop*       )s, sigma);
  if (s->name() ==  "Block"      )  return M((Block*      )s, sigma);
  return NULL;
}


State* Semantics::M(Skip *s, State *sigma) {
  return sigma;
}


State* Semantics::M(Assignment *a, State *sigma) {  
  return sigma->onion(State(a->target, M(a->source, sigma)));
}


State* Semantics::M(Block *b, State *sigma) {
  for (unsigned int i=0; i<b->members.size(); i++) {
    sigma = M(dynamic_cast<Statement*>(b->members[i]), sigma);
  }
  return sigma;
}


State* Semantics::M(Conditional *c, State *sigma) {
  if (M(c->test, sigma).boolValue)
    return M(c->thenbranch, sigma);
  else
    return M(c->elsebranch, sigma);
}


State* Semantics::M(Loop *l, State *sigma) {
  if (M(l->test, sigma).boolValue)
    return M(l, M (l->body, sigma));
  else return sigma;
}

/**
 * Convert an integer to a string
 */
inline std::string Semantics::itoa(const int x) const {
  std::ostringstream o;
  if (!(o << x))
    throw runtime_error("itoa(int)");
  return o.str();
}
